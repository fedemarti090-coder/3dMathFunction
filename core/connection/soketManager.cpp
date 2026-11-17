#include "soketManager.hpp"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

SoketManager::SoketManager() {}

SoketManager::~SoketManager() {
    stop();
}

namespace
{
void closeSocket(SOCKET socketHandle)
{
#ifdef _WIN32
    shutdown(socketHandle, SD_BOTH);
    closesocket(socketHandle);
#else
    shutdown(socketHandle, SHUT_RDWR);
    close(socketHandle);
#endif
}
}

void SoketManager::listenLoop(int port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[SoketManager] Errore inizializzazione Winsock.\n";
        isRunning = false;
        return;
    }
#endif

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "[SoketManager] Errore creazione socket server.\n";
        isRunning = false;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }
    {
        std::lock_guard<std::mutex> lock(socketMutex);
        serverSocketInternal = (void *)(intptr_t)serverSock;
    }

    if (!isRunning) {
        closeConnection();
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSock, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "[SoketManager] Errore bind sulla porta " << port << "\n";
        closeConnection();
        isRunning = false;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    if (listen(serverSock, 1) == SOCKET_ERROR) {
        std::cerr << "[SoketManager] Errore avvio ascolto sulla porta " << port << "\n";
        closeConnection();
        isRunning = false;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    std::cout << "[SoketManager] In ascolto sulla porta " << port << "... In attesa di Electron.\n";

    sockaddr_in clientAddr{};
#ifdef _WIN32
    int clientAddrSize = sizeof(clientAddr);
#else
    socklen_t clientAddrSize = sizeof(clientAddr);
#endif

    SOCKET clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSock == INVALID_SOCKET) {
        if (isRunning) {
            std::cerr << "[SoketManager] Errore accettazione client.\n";
        }
        closeConnection();
        isRunning = false;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    {
        std::lock_guard<std::mutex> lock(socketMutex);
        if (isRunning) {
            clientSocketInternal = (void *)(intptr_t)clientSock;
        }
    }
    if (!isRunning) {
        closeSocket(clientSock);
        closeConnection();
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    std::cout << "[SoketManager] Electron connesso con successo!\n";
    if (onClientConnectedCallback) {
        onClientConnectedCallback();
    }

    char buffer[1024];
    std::string pending;
    while (isRunning) {
        int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            if (isRunning) {
                std::cout << "[SoketManager] Connessione chiusa da Electron.\n";
            }
            break;
        }
        pending.append(buffer, bytesRead);

        size_t newlinePosition = 0;
        while ((newlinePosition = pending.find('\n')) != std::string::npos) {
            std::string rawMessage = pending.substr(0, newlinePosition);
            pending.erase(0, newlinePosition + 1);
            if (!rawMessage.empty() && onMessageCallback) {
                onMessageCallback(rawMessage);
            }
        }

        if (pending.size() > 65536) {
            std::cerr << "[SoketManager] Messaggio ricevuto troppo lungo.\n";
            break;
        }
    }

    closeConnection();
    isRunning = false;
#ifdef _WIN32
    WSACleanup();
#endif
}

void SoketManager::start(int port) {
    if (isRunning.exchange(true)) return;
    listenerThread = std::thread(&SoketManager::listenLoop, this, port);
}

void SoketManager::stop() {
    isRunning = false;
    closeConnection();
    if (listenerThread.joinable() && listenerThread.get_id() != std::this_thread::get_id()) {
        listenerThread.join();
    }
}

void SoketManager::closeConnection() {
    std::lock_guard<std::mutex> lock(socketMutex);
    if (clientSocketInternal != nullptr) {
        SOCKET clientSock = (SOCKET)(intptr_t)clientSocketInternal;
        closeSocket(clientSock);
        clientSocketInternal = nullptr;
    }
    if (serverSocketInternal != nullptr) {
        SOCKET serverSock = (SOCKET)(intptr_t)serverSocketInternal;
        closeSocket(serverSock);
        serverSocketInternal = nullptr;
    }
}

void SoketManager::setOnMessageListener(std::function<void(const std::string &)> callback) {
    onMessageCallback = callback;
}

void SoketManager::setOnClientConnectedListener(std::function<void()> callback) {
    onClientConnectedCallback = callback;
}

bool SoketManager::sendRawData(const std::string &rawData) {
    std::lock_guard<std::mutex> lock(socketMutex);
    if (clientSocketInternal == nullptr || !isRunning) return false;

    SOCKET clientSock = (SOCKET)(intptr_t)clientSocketInternal;
    std::string payload = rawData + "\n";
    size_t sent = 0;
    while (sent < payload.size()) {
        int bytesSent = send(clientSock, payload.data() + sent, static_cast<int>(payload.size() - sent), 0);
        if (bytesSent == SOCKET_ERROR || bytesSent == 0) return false;
        sent += static_cast<size_t>(bytesSent);
    }
    return true;
}
#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

class SoketManager
{
private:
    std::function<void(const std::string &)> onMessageCallback;
    std::function<void()> onClientConnectedCallback;
    void *serverSocketInternal = nullptr; // Puntatore opaco o gestione interna
    void *clientSocketInternal = nullptr;
    std::atomic<bool> isRunning{false};
    std::thread listenerThread;
    std::mutex socketMutex;

    void listenLoop(int port);

public:
    SoketManager();
    ~SoketManager();

    void start(int port = 12345);
    void stop();
    void closeConnection();
    void setOnMessageListener(std::function<void(const std::string &)> callback);
    void setOnClientConnectedListener(std::function<void()> callback);
    bool sendRawData(const std::string &rawData);
};
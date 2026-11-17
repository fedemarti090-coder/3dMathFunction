#include "core/connection/settingBridge.hpp"
#include "core/connection/payloadStruct.hpp"
#include "core/invokeSettingApp.hpp"

#include "core/windowGPU.hpp"
#include <mutex>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    int ans = 0;
    try
    {
        SettingBridge settingBridge;

        DataGet data{};
        std::mutex dataMutex;
        bool hasPendingSettings = false;

        settingBridge.setOnDataReceivedCallback([&](const DataGet &dataReceived)
                                                {
            std::lock_guard<std::mutex> lock(dataMutex);
            data = dataReceived;
            hasPendingSettings = true;
            std::cout << "Impostazioni ricevute da Electron: " << data.function << std::endl; });

        settingBridge.setOnClientConnectedCallback([&settingBridge]()
                                                   {
            DataGive initialData{0.0f, 0.0f};
            if (!settingBridge.sendData(initialData))
            {
                std::cerr << "Impossibile inviare i dati iniziali a Electron.\n";
            } });

        settingBridge.start(1234);

        InvokeApp invokeApp; // Avvio finestra node js

        WinINIT app;

        auto takePendingSettings = [&](DataGet &settings)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            if (!hasPendingSettings)
            {
                return false;
            }

            settings = data;
            hasPendingSettings = false;
            return true;
        };

        std::string function = "sin(x) + cos(y)";
        ans = app.Run(hInstance, nCmdShow, function, settingBridge, takePendingSettings);

        settingBridge.stop();
    }
    catch (const std::exception &e)
    {
        std::cout << "Errore fatale: " << e.what() << std::endl;
        return -1;
    }
    return ans;
}
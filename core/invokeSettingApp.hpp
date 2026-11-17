#include <iostream>
#include <cstdlib>

class InvokeApp
{
public:
    InvokeApp()
    {
        launch();
    }

    void launch()
    {
        std::cout << "[InvokeApp] Avvio di Electron in corso...\n";

#ifdef _WIN32
        system("start cmd.exe /c \"cd /d C:\\Users\\utente\\Desktop\\C++\\__Vulkan\\3dMathFunction\\setting && npm start\"");
#else
        system("cd /path/to/setting && npm start &");
#endif
    }
};
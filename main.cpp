#include "core/windowGPU.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    int ans = 0;
    try
    {
        WinINIT app;

        std::string function = "sin(x^2 + y^2)";
        ans = app.Run(hInstance, nCmdShow, function);
    }
    catch (const std::exception &e)
    {
        std::cout << "Errore fatale: " << e.what() << std::endl;
        return -1;
    }
    return ans;
}
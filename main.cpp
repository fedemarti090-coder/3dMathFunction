#include "core/windowGPU.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    int ans = 0;
    try
    {
        WinINIT app;

        std::string function = "x^3 - 3xy^2";
        ans = app.Run(hInstance, nCmdShow, function);
    }
    catch (const std::exception &e)
    {
        std::cout << "Errore fatale: " << e.what() << std::endl;
        return -1;
    }
    return ans;
}
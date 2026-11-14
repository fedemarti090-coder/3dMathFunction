#include "core/windowGPU.hpp"
#include "core/calc/mathEnigne.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WinINIT app;
    MathEngine mathEnigne;

    mathEnigne.calcolaFunzione3D("x^3-3xy^2");

    return app.Run(hInstance, nCmdShow);
}
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

class WinINIT
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
        }
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    int Run(HINSTANCE hInstance, int nCmdShow)
    {
        // Registrazione classe della finestra
        const wchar_t CLASS_NAME[] = L"Sample Window Class";

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc; // Ora compila perché è static!
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);

        // Creazione della finestra
        HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Finestra vuota",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            NULL, NULL, hInstance, NULL);

        if (hwnd == NULL)
        {
            return 0;
        }

        ShowWindow(hwnd, nCmdShow);

        // Loop dei messaggi ad alte prestazioni (Game Loop)
        MSG msg = {};
        bool isRunning = true;

        while (isRunning)
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    isRunning = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (isRunning)
            {
                // funzioni del MainLoop

                Sleep(16); // ~60 FPS
            }
        }

        return (int)msg.wParam;
    }
};



/*

#include "core/window.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WinINIT app;
    return app.Run(hInstance, nCmdShow);
}


*/
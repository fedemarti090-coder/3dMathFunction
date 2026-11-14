#ifndef UNICODE
#define UNICODE
#endif

#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

class WinINIT
{
private:
    VkInstance instance = VK_NULL_HANDLE;  // Istanza global
    VkSurfaceKHR surface = VK_NULL_HANDLE; // Superfice di redneringo

    int width = 1200;
    int height = 800;

    void initVulkan(HWND hwnd, HINSTANCE hInstance)
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Win32 Vulkan App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        std::vector<const char *> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Impossibile creare l'istanza Vulkan!");
        }

        // Creazione superfice Win32 ( HWND -> Vulkan )

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hwnd = hwnd;
        surfaceCreateInfo.hinstance = hInstance;

        if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Impossibile creare la superficie Vulkan per Win32!");
        }
    }

    void cleanupVulkan()
    {
        if (surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(instance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }
        if (instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0); // Fine del programma
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

        const wchar_t CLASS_NAME[] = L"Vulkan Window Class";

        WNDCLASSW wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

        RegisterClassW(&wc);

        HWND hwnd = CreateWindowExW(
            0, CLASS_NAME, L"Calcolatrice 3D di funzioni grafiche",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr, nullptr, hInstance, nullptr);

        if (hwnd == NULL)
            return 0;

        ShowWindow(hwnd, nCmdShow);

        try
        {
            initVulkan(hwnd, hInstance);
        }
        catch (const std::exception &e)
        {
            MessageBoxA(hwnd, e.what(), "Errore Critico Vulkan", MB_OK | MB_ICONERROR);
            cleanupVulkan();
            return -1;
        }

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

                Sleep(16); // 60 FPS
            }
        }

        cleanupVulkan();

        return (int)msg.wParam;
    }
};
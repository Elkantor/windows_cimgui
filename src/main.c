#include "deps.c"

typedef struct windows_win
{
    HWND windows_wnds[10];
    HMODULE windows_modules[10];
    MSG windows_msgs[10];
} windows_win;

HMODULE window_hmodule_get(windows_win* windows, const char* window_name)
{
    const uintptr_t window_address = (uintptr_t) window_name;
    const size_t windows_count = sizeof(windows->windows_modules) / sizeof(windows->windows_modules[0]);
    const size_t index = (size_t)window_address % windows_count;

    return windows->windows_modules[index];
}

HWND window_wnd_get(windows_win* windows, const char* window_name)
{
    const uintptr_t window_address = (uintptr_t) window_name;
    const size_t windows_count = sizeof(windows->windows_wnds) / sizeof(windows->windows_wnds[0]);
    const size_t index = (size_t)window_address % windows_count;

    return windows->windows_wnds[index];
}

MSG window_msg_get(windows_win* windows, const char* window_name)
{
    const uintptr_t window_address = (uintptr_t) window_name;
    const size_t windows_count = sizeof(windows->windows_msgs) / sizeof(windows->windows_msgs[0]);
    const size_t index = (size_t)window_address % windows_count;

    return windows->windows_msgs[index];
}

bool window_null_all(windows_win* windows)
{
    const size_t windows_count = sizeof(windows->windows_wnds) / sizeof(windows->windows_wnds[0]);
    
    uintptr_t addresses_sum = 0;
    for (size_t i = 0; i < windows_count; ++i)
    {
        addresses_sum += (uintptr_t) windows->windows_wnds[i];
    }
    
    return addresses_sum == 0;
}

LRESULT CALLBACK win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    char window_title[256] = { 0 };
    GetWindowTextA(hwnd, window_title, sizeof(window_title));

    switch(msg)
    {
        case WM_CREATE:
        {
            printf("[LOG]: window %s created\n", window_title);
            return 0;
        }
        case WM_COMMAND:
        {
            return 0;
        }
        case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;
                hdc = BeginPaint(hwnd, &ps);
                SetTextColor(hdc, RGB(255,0,0));
                TextOut(hdc, 0, 0, "Hello, from WM_PAINT event!", 27);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_WINDOWPOSCHANGED:
        {
            if((lparam == 0) || ((((PWINDOWPOS) lparam)->flags & SWP_NOSIZE) == 0))
            {
                InvalidateRect(hwnd, 0, TRUE);
            }
            return 0;
        }
        case WM_KEYDOWN:
        {
            return 0;
        }
        case WM_CLOSE:
        {
            // Get the title of the current window
            printf("[LOG]: destroying the %s window at address: %llu\n", window_title, (uintptr_t)hwnd);
            DestroyWindow(hwnd);
            hwnd = NULL;
            return 0;
        }
        case WM_DESTROY:
        {
            printf("Destroy event\n");
            PostQuitMessage(0);
            return 0;
        }
        case WM_NCACTIVATE:
        {
            return DefWindowProc(hwnd, msg, TRUE, lparam);
        }
        default:
        {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
}


void window_add(windows_win* windows, const char* name, int x, int y, int width, int height)
{
    const uintptr_t window_address = (uintptr_t) name;
    const size_t windows_count = sizeof(windows->windows_wnds) / sizeof(windows->windows_wnds[0]);
    const size_t index = (size_t)window_address % windows_count;

    windows->windows_modules[index] = GetModuleHandle(0);

    HICON hIcon = LoadImage(windows->windows_modules[index], "app.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE);  

    // Step 1: Registering the window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = win_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = windows->windows_modules[index],
        .hIcon = hIcon,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW+1),
        .lpszMenuName = NULL,
        .lpszClassName = name,
        .hIconSm = hIcon,
    };

    // Handle the case where an error occured
    // during the step 1
    if(RegisterClassEx(&wc) == 0)
    {
        MessageBox(NULL, "Window Registration Failed !", "Error !", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // Step 2: Creating the window
    unsigned long long normal = WS_POPUP | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_VISIBLE;
    unsigned long long no_title_bar = WS_POPUPWINDOW | WS_THICKFRAME | WS_VISIBLE | WS_CLIPCHILDREN;
    windows->windows_wnds[index] = CreateWindowEx(WS_EX_APPWINDOW, name, name, no_title_bar, x, y, width, height, NULL, NULL, windows->windows_modules[index], NULL);

    // Handle the case where an error occured 
    // during the step 2
    if(windows->windows_wnds[index] == NULL)
    {
        MessageBox(NULL, "Window Creation Failed !", "Error !", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    STARTUPINFOA startup_info;
    GetStartupInfoA(&startup_info);
    ShowWindow(windows->windows_wnds[index], SW_SHOWDEFAULT);
    UpdateWindow(windows->windows_wnds[index]);

    // The window is now ready to be drawn on the screen
    return;
}

bool window_show_all(windows_win* windows)
{
    const size_t windows_count = sizeof(windows->windows_wnds) / sizeof(windows->windows_wnds[0]);

    unsigned int result = 0;
    for (size_t i = 0; i < windows_count; ++i)
    {
        // Show the window by calling the win32 api functions
        if(GetMessage(&windows->windows_msgs[i], NULL, 0, 0) > 0)
        {
            TranslateMessage(&windows->windows_msgs[i]);
            DispatchMessage(&windows->windows_msgs[i]);
            result += 1;
        }
    }

    printf("[LOG]: result: %d\n", result);

    return result != 0;
}

windows_win windows = { 0 };

int main(int argc, char* argv[])
{
    window_add(&windows, "window_1", 600, 300, 800, 600);
    
    while(window_show_all(&windows) == true)
    {
        
    }

    printf("Exit program\n");

    return 0; 
}

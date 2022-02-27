// #include <windows.h>

// #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
// #include "../../external/cimgui/cimgui.h"

typedef struct imgui_impl_win32_data
{
    HWND                        hWnd;
    HWND                        MouseHwnd;
    bool                        MouseTracked;
    int                         MouseButtonsDown;
    INT64                       Time;
    INT64                       TicksPerSecond;
    ImGuiMouseCursor            LastMouseCursor;
    bool                        HasGamepad;
    bool                        WantUpdateHasGamepad;
} imgui_impl_win32_data;


bool imgui_impl_win32_init(imgui_impl_win32_data* imgui_backend, void* hwnd)
{
    ImGuiIO* io = igGetIO();

    if (io->BackendPlatformUserData != NULL)
    {
        printf("Already initialized a platform backend!");
        exit(1);
    }

    int64_t perf_frequency;
    int64_t perf_counter;
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency))
        return false;
    if (!QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter))
        return false;

    // Setup backend capabilities flags
    io->BackendPlatformUserData = (void*)imgui_backend;
    io->BackendPlatformName = "imgui_impl_win32";
    io->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

    imgui_backend->hWnd = (HWND)hwnd;
    imgui_backend->WantUpdateHasGamepad = true;
    imgui_backend->TicksPerSecond = perf_frequency;
    imgui_backend->Time = perf_counter;
    imgui_backend->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Set platform dependent data in viewport
    igGetMainViewport()->PlatformHandleRaw = (void*)hwnd;

    return true;
}

void imgui_impl_win32_shutdown(imgui_impl_win32_data* imgui_backend)
{
    if (imgui_backend == NULL)
    {
        printf("No platform backend to shutdown, or already shutdown?");
        exit(1);
    }

    ImGuiIO* io = igGetIO();
    io->BackendPlatformName = NULL;
    io->BackendPlatformUserData = NULL;
}



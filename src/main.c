#include "deps.c"

windows_win windows = { 0 };
imgui_impl_win32_data imgui_backend = { 0 };

int main(int argc, char* argv[])
{
    window_add(&windows, "window_1", 600, 300, 800, 600);

    igCreateContext(NULL);
    imgui_impl_win32_init(&imgui_backend, window_wnd_get(&windows, "window_1"));
    
    while(window_show_all(&windows) == true)
    {

    }

    imgui_impl_win32_shutdown(&imgui_backend);
    printf("Exit program\n");

    return 0; 
}

#include "win32_window.h"

#ifdef _WIN32

#include "imgui.h"
#include "imgui_impl_win32.h"

// V0.9.6: ImGui intentionally hides this declaration in a #if 0 block in
// imgui_impl_win32.h to avoid dragging <windows.h> into the header.
// Forward-declare it here so WndProc can call it.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace visionlab::ui {


static const wchar_t* WINDOW_CLASS_NAME = L"VisionLabWindowClass";


Win32Window::Win32Window() = default;


Win32Window::~Win32Window()
{
    destroy();
}



bool Win32Window::create(
    int width,
    int height,
    const std::string& title
)
{
    if (hwnd_ != nullptr)
    {
        return true;
    }


    HINSTANCE instance = GetModuleHandle(nullptr);


    WNDCLASSEXW wc = {};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;


    if (!RegisterClassExW(&wc))
    {
        // Class may already be registered from a previous instance
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS)
        {
            return false;
        }
    }


    std::wstring wide_title(title.begin(), title.end());


    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);


    hwnd_ = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        wide_title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        instance,
        this
    );


    if (hwnd_ == nullptr)
    {
        return false;
    }


    ShowWindow(hwnd_, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_);


    should_close_ = false;


    return true;
}



bool Win32Window::create_floating(
    int width,
    int height,
    const std::string& title
)
{
    if (hwnd_ != nullptr)
    {
        return true;
    }

    is_floating_ = true;

    HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassExW(&wc))
    {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS)
        {
            return false;
        }
    }

    std::wstring wide_title(title.begin(), title.end());

    // Floating: popup + layered + toolwindow (no taskbar icon)
    const DWORD ex_style = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    const DWORD style = WS_POPUP;

    hwnd_ = CreateWindowExW(
        ex_style,
        WINDOW_CLASS_NAME,
        wide_title.c_str(),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        instance,
        this
    );

    if (hwnd_ == nullptr)
    {
        return false;
    }

    SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);

    should_close_ = false;
    return true;
}



void Win32Window::destroy()
{
    if (hwnd_ != nullptr)
    {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}



bool Win32Window::process_messages()
{
    MSG msg;

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT)
        {
            should_close_ = true;
            return false;
        }
    }

    return !should_close_;
}



bool Win32Window::should_close() const
{
    return should_close_;
}



HWND Win32Window::hwnd() const
{
    return hwnd_;
}



LRESULT CALLBACK Win32Window::wnd_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wparam,
    LPARAM lparam
)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true;
    }


    switch (msg)
    {
    case WM_NCHITTEST:
    {
        // For floating windows: allow dragging from entire client area
        Win32Window* self = reinterpret_cast<Win32Window*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (self != nullptr && self->is_floating_)
        {
            return HTCAPTION;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }


    return DefWindowProcW(hwnd, msg, wparam, lparam);
}


}

#endif

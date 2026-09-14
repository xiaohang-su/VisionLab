#pragma once

#ifdef _WIN32

#include <windows.h>

#include <string>


namespace visionlab::ui {


class Win32Window
{

public:

    Win32Window();

    ~Win32Window();


    bool create(
        int width,
        int height,
        const std::string& title
    );

    // Create a borderless floating tool window (WS_POPUP | WS_EX_LAYERED | WS_EX_TOOLWINDOW)
    bool create_floating(
        int width,
        int height,
        const std::string& title
    );

    void destroy();


    bool process_messages();


    bool should_close() const;


    HWND hwnd() const;


private:

    static LRESULT CALLBACK wnd_proc(
        HWND hwnd,
        UINT msg,
        WPARAM wparam,
        LPARAM lparam
    );


    HWND hwnd_ = nullptr;

    bool should_close_ = false;

    bool is_floating_ = false;

};


}

#endif

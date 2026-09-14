#pragma once

#ifdef _WIN32

#include "../ui_renderer.h"
#include "win32_window.h"

#include <d3d11.h>
#include <string>


namespace visionlab::ui {


// Independent floating window with its own D3D11 device and ImGui context.
// Shown after successful login. Pure white minimal style with left nav.
class FloatingWindow
{
public:

    FloatingWindow();
    ~FloatingWindow();

    bool initialize(int width = 900, int height = 560);
    void shutdown();

    bool render(const UIContext& context);
    bool should_close() const;

private:

    bool create_d3d_device();
    void create_frame_texture(int width, int height);
    void upload_frame(const core::types::Frame& frame);
    void setup_style();
    void build_ui(const UIContext& context);

    Win32Window window_;

    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* device_context_ = nullptr;
    IDXGISwapChain* swap_chain_ = nullptr;
    ID3D11RenderTargetView* render_target_view_ = nullptr;

    ID3D11Texture2D* frame_texture_ = nullptr;
    ID3D11ShaderResourceView* frame_srv_ = nullptr;
    int frame_texture_width_ = 0;
    int frame_texture_height_ = 0;

    bool initialized_ = false;

    // Independent ImGui context for this window
    struct ImGuiContext* imgui_context_ = nullptr;

    // Navigation state
    enum class NavPage { AI, Monitor, Models, Settings, About };
    NavPage current_page_ = NavPage::AI;
};


}

#endif

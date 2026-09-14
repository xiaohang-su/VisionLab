#pragma once

#ifdef _WIN32

#include "../ui_renderer.h"
#include "win32_window.h"
#include "floating_window.h"

#include <d3d11.h>

struct ImGuiContext;

namespace visionlab::ui {


class ImGuiRenderer
    : public UIRenderer
{

public:

    ImGuiRenderer();

    ~ImGuiRenderer() override;


    bool initialize(
        int width = 1280,
        int height = 720
    );


    void shutdown();


    const char* name() const override;


    bool render(
        const UIContext& context
    ) override;


    bool should_close() const;

    // Show the floating detection window (called after successful login)
    void show_floating();

    bool floating_visible() const;


private:

    bool create_d3d_device();

    void create_frame_texture(
        int width,
        int height
    );

    void upload_frame(
        const core::types::Frame& frame
    );

    void setup_style();

    void build_ui(
        const UIContext& context
    );

    // V1.1 Platform UI helpers
    void build_top_bar(const UIContext& context);
    void build_side_nav();
    void build_main_area(const UIContext& context);
    void build_bottom_bar(const UIContext& context);
    void build_login_page(const UIContext& context);
    void build_ai_page(const UIContext& context);
    void build_monitor_page(const UIContext& context);
    void build_models_page(const UIContext& context);
    void build_settings_page(const UIContext& context);
    void build_about_page(const UIContext& context);


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

    // Independent ImGui context for main window
    ImGuiContext* imgui_context_ = nullptr;

    // Floating window (shown after login)
    FloatingWindow* floating_ = nullptr;

    // V1.1 Platform UI state
    enum class NavPage { AI, Monitor, Models, Settings, About };
    NavPage current_page_ = NavPage::AI;

    char card_input_[256] = "";
    char login_message_[256] = "";
    bool login_attempted_ = false;

};


}

#endif

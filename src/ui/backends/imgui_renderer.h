#pragma once

#ifdef _WIN32

#include "../ui_renderer.h"
#include "win32_window.h"

#include <d3d11.h>


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

};


}

#endif

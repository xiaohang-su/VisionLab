#include "imgui_renderer.h"

#ifdef _WIN32

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "timer.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <format>


namespace visionlab::ui {


ImGuiRenderer::ImGuiRenderer() = default;


ImGuiRenderer::~ImGuiRenderer()
{
    shutdown();
}



bool ImGuiRenderer::initialize(
    int width,
    int height
)
{
    if (initialized_)
    {
        return true;
    }


    if (!window_.create(width, height, "VisionLab"))
    {
        return false;
    }


    if (!create_d3d_device())
    {
        return false;
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    setup_style();


    ImGui_ImplWin32_Init(window_.hwnd());
    ImGui_ImplDX11_Init(device_, device_context_);


    initialized_ = true;


    return true;
}



void ImGuiRenderer::shutdown()
{
    if (!initialized_)
    {
        return;
    }


    if (frame_srv_ != nullptr)
    {
        frame_srv_->Release();
        frame_srv_ = nullptr;
    }

    if (frame_texture_ != nullptr)
    {
        frame_texture_->Release();
        frame_texture_ = nullptr;
    }


    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();


    if (render_target_view_ != nullptr)
    {
        render_target_view_->Release();
        render_target_view_ = nullptr;
    }

    if (swap_chain_ != nullptr)
    {
        swap_chain_->Release();
        swap_chain_ = nullptr;
    }

    if (device_context_ != nullptr)
    {
        device_context_->Release();
        device_context_ = nullptr;
    }

    if (device_ != nullptr)
    {
        device_->Release();
        device_ = nullptr;
    }


    window_.destroy();


    initialized_ = false;
}



const char* ImGuiRenderer::name() const
{
    return "ImGuiRenderer";
}



bool ImGuiRenderer::render(
    const UIContext& context
)
{
    if (!initialized_)
    {
        return false;
    }


    if (!window_.process_messages() || window_.should_close())
    {
        return false;
    }


    // Upload frame texture (timed: Map + memcpy + Unmap only)
    if (context.frame != nullptr
        && context.frame->width > 0
        && context.frame->height > 0
        && !context.frame->data.empty())
    {
        core::Timer upload_timer;
        upload_frame(*context.frame);
        context.texture_upload_ms = upload_timer.elapsed_ms();
    }
    else
    {
        context.texture_upload_ms = 0.0;
    }


    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    build_ui(context);


    // Render
    ImGui::Render();


    const float clear_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
    device_context_->ClearRenderTargetView(render_target_view_, clear_color);


    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    swap_chain_->Present(1, 0);


    return true;
}



bool ImGuiRenderer::should_close() const
{
    return window_.should_close();
}



bool ImGuiRenderer::create_d3d_device()
{
    DXGI_SWAP_CHAIN_DESC swap_desc = {};

    swap_desc.BufferCount = 2;
    swap_desc.BufferDesc.Width = 0;
    swap_desc.BufferDesc.Height = 0;
    swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_desc.OutputWindow = window_.hwnd();
    swap_desc.SampleDesc.Count = 1;
    swap_desc.SampleDesc.Quality = 0;
    swap_desc.Windowed = TRUE;
    swap_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


    UINT create_device_flags = 0;

#ifdef _DEBUG
    create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


    D3D_FEATURE_LEVEL feature_level;

    const D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };


    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        create_device_flags,
        feature_levels,
        2,
        D3D11_SDK_VERSION,
        &swap_desc,
        &swap_chain_,
        &device_,
        &feature_level,
        &device_context_
    );


    if (FAILED(result))
    {
        return false;
    }


    // Create render target view
    ID3D11Texture2D* back_buffer = nullptr;

    result = swap_chain_->GetBuffer(
        0,
        IID_PPV_ARGS(&back_buffer)
    );


    if (FAILED(result))
    {
        return false;
    }


    result = device_->CreateRenderTargetView(
        back_buffer,
        nullptr,
        &render_target_view_
    );


    back_buffer->Release();


    if (FAILED(result))
    {
        return false;
    }


    return true;
}



void ImGuiRenderer::create_frame_texture(
    int width,
    int height
)
{
    if (frame_texture_ != nullptr)
    {
        frame_srv_->Release();
        frame_srv_ = nullptr;
        frame_texture_->Release();
        frame_texture_ = nullptr;
    }


    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = static_cast<UINT>(width);
    tex_desc.Height = static_cast<UINT>(height);
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DYNAMIC;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tex_desc.MiscFlags = 0;


    device_->CreateTexture2D(
        &tex_desc,
        nullptr,
        &frame_texture_
    );


    device_->CreateShaderResourceView(
        frame_texture_,
        nullptr,
        &frame_srv_
    );


    frame_texture_width_ = width;
    frame_texture_height_ = height;
}



void ImGuiRenderer::upload_frame(
    const core::types::Frame& frame
)
{
    if (frame.width != frame_texture_width_
        || frame.height != frame_texture_height_
        || frame_texture_ == nullptr)
    {
        create_frame_texture(frame.width, frame.height);
    }


    D3D11_MAPPED_SUBRESOURCE mapped;

    HRESULT result = device_context_->Map(
        frame_texture_,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped
    );


    if (FAILED(result))
    {
        return;
    }


    const std::uint8_t* src = frame.data.data();
    std::uint8_t* dst = static_cast<std::uint8_t*>(mapped.pData);

    const UINT src_row_pitch = frame.width * 4; // BGRA8 = 4 bytes per pixel
    const UINT dst_row_pitch = mapped.RowPitch;

    for (UINT y = 0; y < static_cast<UINT>(frame.height); y++)
    {
        std::memcpy(
            dst + y * dst_row_pitch,
            src + y * src_row_pitch,
            src_row_pitch
        );
    }


    device_context_->Unmap(frame_texture_, 0);
}



void ImGuiRenderer::setup_style()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Pure white minimal + rounded corners
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(4, 4);


    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
}



// V0.9.4: Performance Dashboard helpers (UI-local, no core changes)
namespace {

constexpr double NOTICE_RATIO = 0.30;
constexpr double WARNING_RATIO = 0.60;

ImVec4 stage_color(double ratio)
{
    if (ratio > WARNING_RATIO)
    {
        return ImVec4(0.90f, 0.30f, 0.30f, 1.0f); // red
    }
    if (ratio > NOTICE_RATIO)
    {
        return ImVec4(0.95f, 0.75f, 0.20f, 1.0f); // yellow
    }
    return ImVec4(0.30f, 0.75f, 0.40f, 1.0f); // green
}

struct StageCost
{
    const char* name;
    double ms;
};

}



void ImGuiRenderer::build_ui(
    const UIContext& context
)
{
    // Main window
    ImGui::Begin("VisionLab", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);


    // Frame Viewer
    ImGui::BeginChild("FrameViewer", ImVec2(0, -200), true);

    if (frame_srv_ != nullptr && frame_texture_width_ > 0)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();

        float img_width = static_cast<float>(frame_texture_width_);
        float img_height = static_cast<float>(frame_texture_height_);

        float scale = std::min(
            avail.x / img_width,
            avail.y / img_height
        );

        ImVec2 display_size(
            img_width * scale,
            img_height * scale
        );

        ImVec2 cursor = ImGui::GetCursorScreenPos();

        ImGui::Image(
            reinterpret_cast<ImTextureID>(frame_srv_),
            display_size
        );


        // Detection Overlay
        if (context.detections != nullptr)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            for (const auto& det : context.detections->detections)
            {
                ImVec2 p1(
                    cursor.x + det.bbox.x * scale,
                    cursor.y + det.bbox.y * scale
                );
                ImVec2 p2(
                    cursor.x + (det.bbox.x + det.bbox.width) * scale,
                    cursor.y + (det.bbox.y + det.bbox.height) * scale
                );

                draw_list->AddRect(p1, p2, IM_COL32(0, 200, 0, 255), 2.0f);

                std::string label = std::format(
                    "cls {} conf {:.2f}",
                    det.class_id,
                    det.confidence
                );

                draw_list->AddText(p1, IM_COL32(0, 0, 0, 255), label.c_str());
            }
        }


        // Tracking Overlay
        if (context.tracks != nullptr)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            for (const auto& track : context.tracks->tracks)
            {
                if (!track.active) continue;

                ImVec2 p1(
                    cursor.x + track.bbox.x * scale,
                    cursor.y + track.bbox.y * scale
                );
                ImVec2 p2(
                    cursor.x + (track.bbox.x + track.bbox.width) * scale,
                    cursor.y + (track.bbox.y + track.bbox.height) * scale
                );

                draw_list->AddRect(p1, p2, IM_COL32(255, 140, 0, 255), 2.0f);

                std::string label = std::format("ID {}", track.id);

                draw_list->AddText(
                    ImVec2(p1.x, p1.y - 18),
                    IM_COL32(255, 140, 0, 255),
                    label.c_str()
                );
            }
        }
    }
    else
    {
        ImGui::Text("No frame data");
    }

    ImGui::EndChild();


    // Performance Dashboard (V0.9.4)
    ImGui::BeginChild("AnalysisPanel", ImVec2(0, 0), true);

    ImGui::Text("Performance Dashboard");
    ImGui::Separator();

    // --- Analysis Data ---
    if (context.analysis != nullptr)
    {
        ImGui::Text("Detection: %d", context.analysis->detection_count);
        ImGui::Text("Active Tracks: %d", context.analysis->active_track_count);

        for (const auto& stat : context.analysis->class_stats)
        {
            ImGui::Text("  class %d: %d", stat.class_id, stat.count);
        }
    }
    else
    {
        ImGui::Text("No analysis data");
    }


    // --- Runtime Metrics ---
    if (context.metrics != nullptr)
    {
        const auto& m = *context.metrics;

        const double frame_time =
            m.capture_ms + m.vision_ms + m.detection_ms +
            m.tracking_ms + m.analysis_ms + m.render_ms;

        ImGui::Separator();
        ImGui::Text("Frame Time: %.2f ms", frame_time);
        ImGui::Text("FPS: %.1f", context.fps);
        ImGui::Text("Frame: %llu",
            static_cast<unsigned long long>(context.frame_count));


        // --- Stage Timeline ---
        ImGui::Separator();
        ImGui::Text("Stage Timeline");

        StageCost stages[6] = {
            {"Capture",   m.capture_ms},
            {"Vision",    m.vision_ms},
            {"Detection", m.detection_ms},
            {"Tracking",  m.tracking_ms},
            {"Analysis",  m.analysis_ms},
            {"Render",    m.render_ms},
        };

        for (const auto& stage : stages)
        {
            const double ratio =
                (frame_time > 0.0) ? (stage.ms / frame_time) : 0.0;

            const ImVec4 color = stage_color(ratio);

            char label[64];
            std::snprintf(label, sizeof(label),
                "%-10s %6.2f ms", stage.name, stage.ms);

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            ImGui::ProgressBar(static_cast<float>(ratio),
                ImVec2(-1, 0), label);
            ImGui::PopStyleColor();
        }


        // --- Bottleneck Ranking ---
        ImGui::Separator();
        ImGui::Text("Bottleneck Ranking");

        StageCost sorted[6];
        for (int i = 0; i < 6; i++) sorted[i] = stages[i];

        std::sort(sorted, sorted + 6,
            [](const StageCost& a, const StageCost& b) {
                return a.ms > b.ms;
            });

        for (int i = 0; i < 3; i++)
        {
            const double ratio =
                (frame_time > 0.0) ? (sorted[i].ms / frame_time) : 0.0;

            const ImVec4 color = stage_color(ratio);

            ImGui::TextColored(color, "%d. %-10s %5.1f%%",
                i + 1, sorted[i].name, ratio * 100.0);
        }


        // --- Memory Flow ---
        ImGui::Separator();
        ImGui::Text("Memory Flow");
        ImGui::Text("Frame Size:  %.2f MB",
            m.frame_data_bytes / (1024.0 * 1024.0));
        ImGui::Text("Copy Total:  %.2f MB",
            m.frame_copy_bytes / (1024.0 * 1024.0));
        ImGui::Text("Upload Total: %.2f MB (%llu)",
            m.texture_upload_bytes / (1024.0 * 1024.0),
            static_cast<unsigned long long>(m.texture_upload_count));
        ImGui::Text("Upload Time: %.3f ms", m.texture_upload_ms);
        ImGui::Text("Copy BW:     %.1f MB/s", m.vision_copy_bandwidth_MBps);
        ImGui::Text("Allocation:  %llu",
            static_cast<unsigned long long>(m.frame_allocation_count));
        ImGui::Text("Logger:      %.2f ms (%llu calls)",
            m.logger_ms,
            static_cast<unsigned long long>(m.log_count));
    }

    ImGui::EndChild();


    ImGui::End();
}


}

#endif

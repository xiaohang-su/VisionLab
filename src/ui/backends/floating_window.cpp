#include "floating_window.h"

#ifdef _WIN32

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "core/timer.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdio>


namespace visionlab::ui {


FloatingWindow::FloatingWindow() = default;


FloatingWindow::~FloatingWindow()
{
    shutdown();
}


bool FloatingWindow::initialize(int width, int height)
{
    if (initialized_) return true;

    if (!window_.create_floating(width, height, "VisionLab"))
    {
        return false;
    }

    if (!create_d3d_device())
    {
        window_.destroy();
        return false;
    }

    IMGUI_CHECKVERSION();
    imgui_context_ = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context_);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    setup_style();

    ImGui_ImplWin32_Init(window_.hwnd());
    ImGui_ImplDX11_Init(device_, device_context_);

    initialized_ = true;
    return true;
}


void FloatingWindow::shutdown()
{
    if (!initialized_) return;

    if (frame_srv_ != nullptr) { frame_srv_->Release(); frame_srv_ = nullptr; }
    if (frame_texture_ != nullptr) { frame_texture_->Release(); frame_texture_ = nullptr; }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (render_target_view_ != nullptr) { render_target_view_->Release(); render_target_view_ = nullptr; }
    if (swap_chain_ != nullptr) { swap_chain_->Release(); swap_chain_ = nullptr; }
    if (device_context_ != nullptr) { device_context_->Release(); device_context_ = nullptr; }
    if (device_ != nullptr) { device_->Release(); device_ = nullptr; }

    window_.destroy();
    initialized_ = false;
}


bool FloatingWindow::create_d3d_device()
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

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
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

    if (FAILED(hr))
    {
        return false;
    }

    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain_->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (back_buffer == nullptr) return false;

    device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_view_);
    back_buffer->Release();

    return true;
}


void FloatingWindow::create_frame_texture(int width, int height)
{
    if (frame_texture_width_ == width && frame_texture_height_ == height
        && frame_texture_ != nullptr)
    {
        return;
    }

    if (frame_srv_ != nullptr) { frame_srv_->Release(); frame_srv_ = nullptr; }
    if (frame_texture_ != nullptr) { frame_texture_->Release(); frame_texture_ = nullptr; }

    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DYNAMIC;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tex_desc.MiscFlags = 0;

    device_->CreateTexture2D(&tex_desc, nullptr, &frame_texture_);

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;

    device_->CreateShaderResourceView(frame_texture_, &srv_desc, &frame_srv_);

    frame_texture_width_ = width;
    frame_texture_height_ = height;
}


void FloatingWindow::upload_frame(const core::types::Frame& frame)
{
    if (frame.width == 0 || frame.height == 0 || frame.data.empty()) return;

    create_frame_texture(frame.width, frame.height);
    if (frame_texture_ == nullptr) return;

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = device_context_->Map(
        frame_texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    if (FAILED(hr)) return;

    const uint8_t* src = frame.data.data();
    uint8_t* dst = static_cast<uint8_t*>(mapped.pData);

    const size_t src_pitch = static_cast<size_t>(frame.width) * 4;
    const size_t dst_pitch = mapped.RowPitch;

    for (uint32_t y = 0; y < frame.height; y++)
    {
        std::memcpy(dst + y * dst_pitch, src + y * src_pitch, src_pitch);
    }

    device_context_->Unmap(frame_texture_, 0);
}


void FloatingWindow::setup_style()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Pure white minimal
    style.WindowRounding = 12.0f;
    style.FrameRounding = 6.0f;
    style.ChildRounding = 8.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.97f, 0.97f, 0.98f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.93f, 0.93f, 0.95f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.88f, 0.88f, 0.92f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.82f, 0.88f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.92f, 0.92f, 0.96f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.92f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.78f, 0.88f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.15f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.85f, 0.85f, 0.90f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.88f, 0.88f, 0.92f, 1.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.30f, 0.55f, 0.95f, 1.0f);
}


bool FloatingWindow::render(const UIContext& context)
{
    if (!initialized_) return false;

    if (!window_.process_messages() || window_.should_close())
    {
        return false;
    }

    // Switch to this window's ImGui context
    if (imgui_context_ != nullptr)
    {
        ImGui::SetCurrentContext(imgui_context_);
    }

    // Upload frame
    if (context.frame != nullptr
        && context.frame->width > 0
        && context.frame->height > 0
        && !context.frame->data.empty())
    {
        upload_frame(*context.frame);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    build_ui(context);

    ImGui::Render();

    const float clear_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
    device_context_->ClearRenderTargetView(render_target_view_, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swap_chain_->Present(1, 0);

    return true;
}


bool FloatingWindow::should_close() const
{
    return window_.should_close();
}


void FloatingWindow::build_ui(const UIContext& context)
{
    // Full-screen floating window (no title bar)
    ImGui::Begin("Floating", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const float nav_width = 120.0f;

    // === Left Navigation ===
    ImGui::BeginChild("Nav", ImVec2(nav_width, 0), true);

    ImGui::Spacing();
    ImGui::Text("  VisionLab");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    struct NavItem {
        const char* label;
        NavPage page;
        const char* icon;
    };

    const NavItem items[] = {
        {"AI",     NavPage::AI,       "🎯"},
        {"监控",   NavPage::Monitor,  "📊"},
        {"模型",   NavPage::Models,   "🧠"},
        {"设置",   NavPage::Settings, "⚙️"},
        {"说明",   NavPage::About,    "ℹ️"},
    };

    for (const auto& item : items)
    {
        const bool selected = (current_page_ == item.page);
        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.88f, 0.90f, 0.98f, 1.0f));
        }

        if (ImGui::Button(
            (std::string(item.icon) + " " + item.label).c_str(),
            ImVec2(-1, 34)))
        {
            current_page_ = item.page;
        }

        if (selected) ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // === Right Content Area ===
    ImGui::BeginChild("Content", ImVec2(0, 0), true);

    switch (current_page_)
    {
        case NavPage::AI:
        {
            // Frame viewer with overlays
            if (frame_srv_ != nullptr && frame_texture_width_ > 0)
            {
                ImVec2 avail = ImGui::GetContentRegionAvail();
                float img_w = static_cast<float>(frame_texture_width_);
                float img_h = static_cast<float>(frame_texture_height_);
                float scale = std::min(avail.x / img_w, avail.y / img_h);
                ImVec2 display_size(img_w * scale, img_h * scale);
                ImVec2 cursor = ImGui::GetCursorScreenPos();

                ImGui::Image(
                    reinterpret_cast<ImTextureID>(frame_srv_),
                    display_size);

                // Detection overlay
                if (context.detections != nullptr)
                {
                    ImDrawList* dl = ImGui::GetWindowDrawList();
                    for (const auto& det : context.detections->detections)
                    {
                        ImVec2 p1(cursor.x + det.bbox.x * scale,
                                 cursor.y + det.bbox.y * scale);
                        ImVec2 p2(cursor.x + (det.bbox.x + det.bbox.width) * scale,
                                 cursor.y + (det.bbox.y + det.bbox.height) * scale);
                        dl->AddRect(p1, p2, IM_COL32(0, 200, 0, 255), 2.0f);
                        char label[64];
                        snprintf(label, sizeof(label), "cls %d conf %.2f", det.class_id, det.confidence);
                        dl->AddText(p1, IM_COL32(0, 0, 0, 255), label);
                    }
                }

                // Tracking overlay
                if (context.tracks != nullptr)
                {
                    ImDrawList* dl = ImGui::GetWindowDrawList();
                    for (const auto& track : context.tracks->tracks)
                    {
                        if (!track.active) continue;
                        ImVec2 p1(cursor.x + track.bbox.x * scale,
                                 cursor.y + track.bbox.y * scale);
                        ImVec2 p2(cursor.x + (track.bbox.x + track.bbox.width) * scale,
                                 cursor.y + (track.bbox.y + track.bbox.height) * scale);
                        dl->AddRect(p1, p2, IM_COL32(255, 140, 0, 255), 2.0f);
                        char label[32];
                        snprintf(label, sizeof(label), "ID %d", track.id);
                        dl->AddText(ImVec2(p1.x, p1.y - 18),
                            IM_COL32(255, 140, 0, 255), label);
                    }
                }
            }
            else
            {
                ImGui::Text("等待画面...");
            }

            // Quick stats
            ImGui::Spacing();
            if (context.analysis != nullptr)
            {
                ImGui::Text("检测: %d  跟踪: %d  FPS: %.1f",
                    context.analysis->detection_count,
                    context.analysis->active_track_count,
                    context.fps);
            }
            break;
        }

        case NavPage::Monitor:
        {
            ImGui::Text("性能监控");
            ImGui::Separator();
            ImGui::Spacing();

            if (context.metrics != nullptr)
            {
                const auto& m = *context.metrics;
                ImGui::Text("FPS: %.1f", context.fps);
                ImGui::Text("Capture: %.2f ms", m.capture_ms);
                ImGui::Text("Vision: %.2f ms", m.vision_ms);
                ImGui::Text("Detection: %.2f ms", m.detection_ms);
                ImGui::Text("Tracking: %.2f ms", m.tracking_ms);
                ImGui::Text("Analysis: %.2f ms", m.analysis_ms);
                ImGui::Text("Render: %.2f ms", m.render_ms);
            }
            else
            {
                ImGui::Text("无性能数据");
            }
            break;
        }

        case NavPage::Models:
        {
            ImGui::Text("模型管理");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("当前模型: Mock (无真实模型)");
            ImGui::Spacing();
            ImGui::Text("支持格式:");
            ImGui::BulletText("ONNX (.onnx) - YOLOv5/v8");
            ImGui::BulletText("TensorRT (.engine) - 未来");
            ImGui::Spacing();
            ImGui::Text("推理后端:");
            ImGui::BulletText("Mock (当前)");
            ImGui::BulletText("ONNX Runtime CPU - 待接入");
            ImGui::BulletText("ONNX Runtime CUDA - 待接入");
            ImGui::BulletText("TensorRT - 待接入");
            break;
        }

        case NavPage::Settings:
        {
            ImGui::Text("设置");
            ImGui::Separator();
            ImGui::Spacing();

            static float conf = 0.25f;
            static float iou = 0.45f;
            ImGui::SliderFloat("置信度阈值", &conf, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("IOU 阈值", &iou, 0.0f, 1.0f, "%.2f");

            ImGui::Spacing();
            ImGui::Separator();
            static bool show_det = true;
            static bool show_trk = true;
            ImGui::Checkbox("显示检测框", &show_det);
            ImGui::Checkbox("显示跟踪ID", &show_trk);
            break;
        }

        case NavPage::About:
        {
            ImGui::Text("关于 VisionLab");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("版本: 1.1.0 (悬浮窗 Demo)");
            ImGui::Text("语言: C++20");
            ImGui::Text("UI: Dear ImGui + DirectX11");
            ImGui::Text("平台: Windows 10/11");
            ImGui::Spacing();
            ImGui::Text("定位: 模块化 AI 计算机视觉平台");
            break;
        }
    }

    ImGui::EndChild();
    ImGui::End();
}


}

#endif

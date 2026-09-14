#include "screen_capture_source.h"

// V0.9.6: Windows Graphics Capture requires MSVC + Windows SDK (C++/WinRT).
// MinGW/Clang on Windows do not ship these headers.
#if defined(_WIN32) && defined(_MSC_VER) && defined(VISIONLAB_ENABLE_SCREEN_CAPTURE)

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <windows.graphics.capture.h>
#include <windows.graphics.directx.direct3d11.h>
#include <windows.graphics.directx.direct3d11.interop.h>

#include <cstring>
#include <vector>

namespace visionlab::capture {

using Microsoft::WRL::ComPtr;

namespace abi = ABI::Windows::Graphics::Capture;
namespace abi_dx = ABI::Windows::Graphics::DirectX::Direct3D11;


struct ScreenCaptureSource::Impl
{
    ComPtr<ID3D11Device> d3d_device;
    ComPtr<ID3D11DeviceContext> d3d_context;
    ComPtr<abi_dx::IDirect3DDevice> direct3d_device;

    ComPtr<abi::IGraphicsCaptureItem> capture_item;
    ComPtr<abi::IDirect3D11CaptureFramePool> frame_pool;
    ComPtr<abi::IGraphicsCaptureSession> capture_session;

    ComPtr<ID3D11Texture2D> staging_texture;
    UINT staging_width = 0;
    UINT staging_height = 0;

    bool opened = false;
};


ScreenCaptureSource::ScreenCaptureSource() = default;


ScreenCaptureSource::~ScreenCaptureSource()
{
    close();
}


bool ScreenCaptureSource::open()
{
    if (impl_ != nullptr)
    {
        close();
    }

    impl_ = new Impl();
    Impl* impl = static_cast<Impl*>(impl_);

    // 1. Create D3D11 device with BGRA support
    UINT create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        create_flags,
        feature_levels,
        ARRAYSIZE(feature_levels),
        D3D11_SDK_VERSION,
        &impl->d3d_device,
        nullptr,
        &impl->d3d_context
    );

    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 2. Wrap D3D11 device as WinRT IDirect3DDevice
    ComPtr<IDXGIDevice> dxgi_device;
    hr = impl->d3d_device.As(&dxgi_device);
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    hr = CreateDirect3D11DeviceFromDXGIDevice(
        dxgi_device.Get(),
        &impl->direct3d_device
    );
    if (FAILED(hr) || impl->direct3d_device == nullptr)
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 3. Get primary monitor
    HMONITOR monitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    if (monitor == nullptr)
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 4. Create GraphicsCaptureItem from monitor
    ComPtr<abi::IGraphicsCaptureItemStatics> item_statics;
    hr = Windows::Foundation::GetActivationFactory(
        Microsoft::WRL::Wrappers::HStringReference(
            RuntimeClass_Windows_Graphics_Capture_GraphicsCaptureItem
        ).Get(),
        &item_statics
    );
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    hr = item_statics->CreateFromMonitor(monitor, &impl->capture_item);
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 5. Get monitor size
    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(monitor_info);
    if (!GetMonitorInfoW(monitor, &monitor_info))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    UINT width = static_cast<UINT>(
        monitor_info.rcMonitor.right - monitor_info.rcMonitor.left
    );
    UINT height = static_cast<UINT>(
        monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top
    );

    // 6. Create Direct3D11CaptureFramePool
    ComPtr<abi::IDirect3D11CaptureFramePoolStatics> pool_statics;

    hr = Windows::Foundation::GetActivationFactory(
        Microsoft::WRL::Wrappers::HStringReference(
            RuntimeClass_Windows_Graphics_Capture_Direct3D11CaptureFramePool
        ).Get(),
        &pool_statics
    );
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    hr = pool_statics->Create(
        impl->direct3d_device.Get(),
        abi_dx::DirectXPixelFormat_B8G8R8A8UIntNormalized,
        2,
        { static_cast<INT32>(width), static_cast<INT32>(height) },
        &impl->frame_pool
    );

    if (FAILED(hr) || impl->frame_pool == nullptr)
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 7. Create capture session
    hr = impl->frame_pool->CreateCaptureSession(
        impl->capture_item.Get(),
        &impl->capture_session
    );
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    // 8. Start capture
    hr = impl->capture_session->StartCapture();
    if (FAILED(hr))
    {
        delete impl;
        impl_ = nullptr;
        return false;
    }

    impl->opened = true;
    return true;
}


bool ScreenCaptureSource::close()
{
    if (impl_ == nullptr)
    {
        return true;
    }

    Impl* impl = static_cast<Impl*>(impl_);

    if (impl->capture_session != nullptr)
    {
        impl->capture_session->Close();
        impl->capture_session.Reset();
    }

    if (impl->frame_pool != nullptr)
    {
        impl->frame_pool->Close();
        impl->frame_pool.Reset();
    }

    impl->capture_item.Reset();
    impl->staging_texture.Reset();
    impl->d3d_context.Reset();
    impl->d3d_device.Reset();

    delete impl;
    impl_ = nullptr;

    return true;
}


bool ScreenCaptureSource::read(
    core::types::Frame& frame
)
{
    if (impl_ == nullptr)
    {
        return false;
    }

    Impl* impl = static_cast<Impl*>(impl_);

    if (!impl->opened || impl->frame_pool == nullptr)
    {
        return false;
    }

    // 1. Try get next frame (non-blocking)
    ComPtr<abi::IDirect3D11CaptureFrame> capture_frame;
    HRESULT hr = impl->frame_pool->TryGetNextFrame(&capture_frame);

    if (FAILED(hr) || capture_frame == nullptr)
    {
        return false;  // No new frame available
    }

    // 2. Get surface from frame
    ComPtr<abi_dx::IDirect3DSurface> direct3d_surface;
    hr = capture_frame->get_Surface(&direct3d_surface);
    if (FAILED(hr))
    {
        return false;
    }

    ComPtr<abi_dx::IDirect3DDxgiInterfaceAccess> dxgi_access;
    hr = direct3d_surface.As(&dxgi_access);
    if (FAILED(hr))
    {
        return false;
    }

    ComPtr<IDXGISurface> dxgi_surface;
    hr = dxgi_access->GetInterface(IID_PPV_ARGS(&dxgi_surface));
    if (FAILED(hr))
    {
        return false;
    }

    ComPtr<ID3D11Texture2D> source_texture;
    hr = dxgi_surface.As(&source_texture);
    if (FAILED(hr))
    {
        return false;
    }

    // 3. Get texture description
    D3D11_TEXTURE2D_DESC texture_desc = {};
    source_texture->GetDesc(&texture_desc);

    // 4. Create or reuse staging texture
    if (impl->staging_texture == nullptr ||
        impl->staging_width != texture_desc.Width ||
        impl->staging_height != texture_desc.Height)
    {
        D3D11_TEXTURE2D_DESC staging_desc = {};
        staging_desc.Width = texture_desc.Width;
        staging_desc.Height = texture_desc.Height;
        staging_desc.MipLevels = 1;
        staging_desc.ArraySize = 1;
        staging_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        staging_desc.SampleDesc.Count = 1;
        staging_desc.Usage = D3D11_USAGE_STAGING;
        staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        hr = impl->d3d_device->CreateTexture2D(
            &staging_desc,
            nullptr,
            &impl->staging_texture
        );
        if (FAILED(hr))
        {
            return false;
        }

        impl->staging_width = texture_desc.Width;
        impl->staging_height = texture_desc.Height;
    }

    // 5. Copy GPU texture to staging
    impl->d3d_context->CopyResource(
        impl->staging_texture.Get(),
        source_texture.Get()
    );

    // 6. Map staging texture and copy to Frame.data
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    hr = impl->d3d_context->Map(
        impl->staging_texture.Get(),
        0,
        D3D11_MAP_READ,
        0,
        &mapped
    );
    if (FAILED(hr))
    {
        return false;
    }

    frame.width = texture_desc.Width;
    frame.height = texture_desc.Height;
    frame.format = core::types::PixelFormat::BGRA8;

    const std::size_t row_size =
        static_cast<std::size_t>(texture_desc.Width) * 4; // BGRA8 = 4 bytes
    const std::size_t total_size =
        row_size * static_cast<std::size_t>(texture_desc.Height);

    frame.data.resize(total_size);

    const std::uint8_t* src =
        static_cast<const std::uint8_t*>(mapped.pData);

    for (UINT y = 0; y < texture_desc.Height; ++y)
    {
        std::memcpy(
            frame.data.data() + y * row_size,
            src + y * mapped.RowPitch,
            row_size
        );
    }

    impl->d3d_context->Unmap(impl->staging_texture.Get(), 0);

    return true;
}


} // namespace visionlab::capture

#else // !VISIONLAB_ENABLE_SCREEN_CAPTURE

// Non-Windows stub: ScreenCaptureSource is not available on this platform.
// The header remains cross-platform; the .cpp is excluded from build by CMake.

namespace visionlab::capture {

ScreenCaptureSource::ScreenCaptureSource() = default;
ScreenCaptureSource::~ScreenCaptureSource() = default;

bool ScreenCaptureSource::open() { return false; }
bool ScreenCaptureSource::close() { return true; }
bool ScreenCaptureSource::read(core::types::Frame&) { return false; }

} // namespace visionlab::capture

#endif // VISIONLAB_ENABLE_SCREEN_CAPTURE

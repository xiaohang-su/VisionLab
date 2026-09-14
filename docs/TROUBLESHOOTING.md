# VisionLab Troubleshooting Guide

## Build Errors

### CMake: "VisionLab requires Windows 10/11"

**Cause**: `CMakeLists.txt` has a hard guard that fails on non-Windows platforms.

**Solution**: This is by design. Build on Windows, or see [BUILD_LINUX.md](BUILD_LINUX.md)
for Linux development workflow (syntax check only).

---

### fatal error: runtime/runtime.h: No such file or directory

**Cause**: CMakeLists.txt was missing `target_include_directories(... src)`.
This was fixed in V0.9.6. If you see this error, you are on an older version.

**Solution**: Update to V0.9.6 or later. The fix adds:
```cmake
target_include_directories(VisionLab PRIVATE ${PROJECT_SOURCE_DIR}/src)
```

---

### fatal error: windows.graphics.capture.h: No such file or directory

**Cause**: Compiling `screen_capture_source.cpp` with MinGW or an older
Windows SDK that lacks Windows Graphics Capture (WinRT) headers.

**Solution**: 
- Use MSVC (Visual Studio 2022) with Windows SDK 10.0.19041+
- Or disable ScreenCapture: do not set `-DVISIONLAB_ENABLE_SCREEN_CAPTURE=ON`
- MinGW builds automatically exclude this file (V0.9.6+)

---

### 'ImGui_ImplWin32_WndProcHandler' was not declared

**Cause**: ImGui intentionally hides this function declaration in a `#if 0`
block in `imgui_impl_win32.h` to avoid dragging `<windows.h>` into the header.

**Solution**: This was fixed in V0.9.6 by adding a forward declaration in
`win32_window.cpp`. If you see this error, update to V0.9.6+.

---

### undefined reference to `D3DCompile`

**Cause**: Missing `d3dcompiler.lib` in linker inputs. ImGui's DX11 backend
calls `D3DCompile()` to compile shaders.

**Solution**: This was fixed in V0.9.6. Ensure `d3dcompiler.lib` is in
`target_link_libraries`.

---

### undefined reference to `DwmIsCompositionEnabled`

**Cause**: Missing `dwmapi.lib` in linker inputs. ImGui's Win32 backend
queries DWM composition state.

**Solution**: This was fixed in V0.9.6. Ensure `dwmapi.lib` is in
`target_link_libraries`.

---

### fatal error: timer.h: No such file or directory

**Cause**: `imgui_renderer.cpp` used `#include "timer.h"` instead of
`#include "core/timer.h"`.

**Solution**: This was fixed in V0.9.6. The correct path is:
```cpp
#include "core/timer.h"
```

---

### error: 'format' is not a member of 'std'

**Cause**: `std::format` requires full C++20 STL support. Older MSVC versions
(VS 2022 before 17.10) may not have it.

**Solution**: 
- Update to Visual Studio 2022 17.10 or later
- Or replace `std::format` with `std::snprintf` in `imgui_renderer.cpp`
  (2 locations)

---

## Runtime Issues

### Window does not appear

**Checklist**:
1. Is `VISIONLAB_UI_IMGUI=ON` set?
2. Did the build succeed with 0 errors?
3. Check console output for D3D11 initialization errors
4. Verify GPU supports DirectX 11 (feature level 11.0+)

---

### Window appears but is black

**Possible causes**:
1. Render target view not created correctly
2. Swap chain present not called
3. ImGui frame not rendered

**Debug steps**:
1. Enable D3D11 debug layer:
   ```cpp
   D3D11CreateDevice(..., D3D11_CREATE_DEVICE_DEBUG, ...)
   ```
2. Check `IDXGISwapChain::Present()` return value
3. Verify `ImGui_ImplDX11_RenderDrawData()` is called

---

### UI is blurry on high-DPI display

**Cause**: High DPI awareness not enabled.

**Solution** (future release):
- Call `ImGui_ImplWin32_EnableDpiAwareness()`
- Add `dpiAware` to application manifest
- This is tracked as a known issue, not yet fixed

---

### Window resize causes stretching/clipping

**Cause**: Swap chain and render target do not resize with the window.

**Solution** (future release):
- Handle `WM_SIZE` in WndProc
- Call `IDXGISwapChain::ResizeBuffers()`
- Recreate render target view
- This is tracked as a known issue, not yet fixed

---

### Crash on exit

**Possible causes**:
1. D3D11 resources released in wrong order
2. ImGui context not shut down before device release
3. Window destroyed while still processing messages

**Correct shutdown order** (implemented in `ImGuiRenderer::shutdown()`):
1. `ImGui_ImplDX11_Shutdown()`
2. `ImGui_ImplWin32_Shutdown()`
3. `ImGui::DestroyContext()`
4. Release D3D11 resources (RTV, device context, swap chain, device)
5. `window_.destroy()`

---

## Architecture Questions

### Why is ScreenCapture disabled by default?

Windows Graphics Capture requires:
- MSVC compiler (not MinGW/Clang)
- Windows SDK with C++/WinRT headers
- Windows 10 2004+ or Windows 11

It is gated behind `-DVISIONLAB_ENABLE_SCREEN_CAPTURE=ON` to keep the
default build simple and portable.

### Why does CMake fail on Linux?

VisionLab targets Windows exclusively. The guard prevents accidental builds
that would fail at link time (missing DirectX libraries). Linux is used for
development and syntax checking only.

### Can I use MinGW instead of MSVC?

- **ImGui backend**: Yes, MinGW-w64 can build the ImGui + DX11 renderer
- **ScreenCapture**: No, MinGW lacks Windows Graphics Capture headers
- **Recommended**: MSVC for full feature set

---

## Getting Help

1. Check this document for your error
2. Check [BUILD_WINDOWS.md](BUILD_WINDOWS.md) or [BUILD_LINUX.md](BUILD_LINUX.md)
3. Review the Known Issues section in BUILD_WINDOWS.md
4. Ensure you are on the latest release

# VisionLab Windows Build Guide

## Requirements

- **OS**: Windows 10 (2004+) or Windows 11
- **Compiler**: Visual Studio 2022 (17.0+), Desktop development with C++ workload
  - `std::format` is used in `imgui_renderer.cpp`; full support requires VS 2022 17.10+
- **Windows SDK**: 10.0.19041.0 or later
- **CMake**: 3.22 or later
- **C++ Standard**: C++20

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `VISIONLAB_UI_IMGUI` | OFF | Enable ImGui + DirectX11 UI renderer |
| `VISIONLAB_ENABLE_SCREEN_CAPTURE` | OFF | Enable Windows Graphics Capture (MSVC only) |

## Configure

### Default build (MockUI, no ImGui, no ScreenCapture)

```cmd
cmake -B build -S .
```

### ImGui DX11 UI backend

```cmd
cmake -B build -S . -DVISIONLAB_UI_IMGUI=ON
```

This enables:
- `third_party/imgui/` compilation
- `src/ui/backends/` (win32_window + imgui_renderer)
- `VISIONLAB_HAS_IMGUI` preprocessor definition
- Links: `d3d11.lib`, `dxgi.lib`, `d3dcompiler.lib`, `dwmapi.lib`, `windowsapp.lib`

### Full build (ImGui + ScreenCapture)

```cmd
cmake -B build -S . -DVISIONLAB_UI_IMGUI=ON -DVISIONLAB_ENABLE_SCREEN_CAPTURE=ON
```

> **Note**: `VISIONLAB_ENABLE_SCREEN_CAPTURE` requires MSVC + Windows SDK with C++/WinRT support. It is automatically disabled on MinGW/Clang.

## Build

```cmd
cmake --build build --config Release
```

Output binary: `build/bin/Release/VisionLab.exe`

## Run

```cmd
build\bin\Release\VisionLab.exe
```

With ImGui backend: a 1280x720 window opens showing the VisionLab UI.

## Verification Checklist

- [ ] CMake configure succeeds (both ON and OFF modes)
- [ ] Build succeeds with 0 errors
- [ ] Win32 window created successfully
- [ ] D3D11 device and swap chain initialized
- [ ] ImGui context created and renders
- [ ] Frame viewer displays captured frame
- [ ] Detection overlay visible (green bounding boxes)
- [ ] Tracking overlay visible (orange boxes + track IDs)
- [ ] Performance Dashboard shows stage timing
- [ ] FPS counter updates
- [ ] Metrics (capture/vision/detection/tracking/analysis/render ms) update
- [ ] Memory flow panel shows frame size, copy, upload
- [ ] Window closes cleanly (no crash on exit)
- [ ] No D3D11 debug layer errors

## Known Issues

### 1. High DPI not handled

- **Impact**: UI may appear blurry or incorrectly scaled on 4K/high-DPI displays.
- **Future fix**: `ImGui_ImplWin32_EnableDpiAwareness()` + per-monitor DPI awareness manifest.

### 2. WM_SIZE / SwapChain resize not handled

- **Impact**: Resizing the window does not resize the swap chain or render target. The frame viewer area may stretch or clip.
- **Future fix**: Handle `WM_SIZE` in WndProc, call `IDXGISwapChain::ResizeBuffers()`, recreate render target view.

### 3. D3D Device Lost not handled

- **Impact**: GPU reset or driver update will cause crash instead of graceful recovery.
- **Future fix**: Check `DXGI_ERROR_DEVICE_REMOVED` from `Present()`, recreate device and resources.

### 4. std::format version requirement

- **Impact**: `imgui_renderer.cpp` uses `std::format` (2 locations). Requires MSVC with full C++20 STL support.
- **Minimum**: Visual Studio 2022 17.0 (partial); recommended 17.10+ (full).
- **Alternative**: Replace with `std::snprintf` if older compiler support is needed.

### 5. MSVC real build not verified

- **Impact**: V0.9.x releases were developed on Linux (g++ syntax check). MinGW-w64 build verified in V0.9.5; MSVC real compilation and runtime testing still pending.
- **Action required**: Build and test on Windows with Visual Studio using the steps above.

## Architecture Constraints

- Core, Runtime, Capture, Vision, Detection, Tracking, Analysis modules: **no Windows dependencies**
- Windows-specific code is isolated to:
  - `src/capture/screen_capture_source.cpp` (D3D11 + Windows Graphics Capture, MSVC only)
  - `src/ui/backends/win32_window.cpp` (Win32 window)
  - `src/ui/backends/imgui_renderer.cpp` (D3D11 + ImGui)
- `screen_capture_source.h` uses `void* impl_` pImpl — no Windows types in public header
- CMake `if(WIN32)` guards Windows linking; `if(MSVC AND VISIONLAB_ENABLE_SCREEN_CAPTURE)` guards ScreenCapture compilation

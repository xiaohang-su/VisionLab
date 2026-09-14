# VisionLab Linux Build Guide

> **Note**: VisionLab is a Windows-targeted platform. Linux is used for
> development, syntax checking, and architecture validation. The full
> ImGui + DirectX11 UI renderer cannot run on Linux.

## Requirements

- **OS**: Linux (tested on Ubuntu 22.04)
- **Compiler**: g++ 11.4+ or clang 14+ (C++20 support)
- **CMake**: 3.22 or later
- **C++ Standard**: C++20

## Important Limitation

`CMakeLists.txt` contains:

```cmake
if(NOT WIN32)
    message(FATAL_ERROR "VisionLab requires Windows 10/11")
endif()
```

This means **CMake configure will fail on Linux** by design. The project
targets Windows and links DirectX libraries.

## Development Workflow on Linux

### 1. Syntax check all source files

```bash
find src -name "*.cpp" -exec g++ -std=c++20 -fsyntax-only -I src {} \;
```

This verifies that all code compiles under C++20 without Windows headers.
Windows-specific files (`screen_capture_source.cpp`, `win32_window.cpp`,
`imgui_renderer.cpp`) compile their non-Windows stub paths.

### 2. CMake structure validation (optional)

To validate CMakeLists.txt structure without the Windows guard, temporarily
comment out the `FATAL_ERROR` line:

```bash
# Temporarily disable the guard
sed -i 's/message(FATAL_ERROR/message(WARNING/' CMakeLists.txt

# Configure (ImGui OFF)
cmake -B build_linux -S .

# Restore
sed -i 's/message(WARNING/message(FATAL_ERROR/' CMakeLists.txt
```

This validates source lists, include paths, and option logic. Do not commit
this temporary change.

### 3. Static analysis

```bash
# Check for common issues
g++ -std=c++20 -Wall -Wextra -fsyntax-only -I src src/main.cpp
```

## What Works on Linux

- Full syntax checking of all modules
- CMake structure validation (with guard disabled)
- Architecture review (dependency direction, module boundaries)
- Git operations and version management

## What Does NOT Work on Linux

- Full CMake build (Windows guard)
- DirectX11 rendering
- Win32 window creation
- Windows Graphics Capture
- ImGui DX11 backend runtime
- Real frame capture and display

## Cross-Compilation (Not Supported)

MinGW-w64 cross-compilation from Linux is **not officially supported**.
The Windows Graphics Capture API requires MSVC + Windows SDK headers that
MinGW does not ship. Use a real Windows environment for Windows builds.

## Recommended Windows Build

See [BUILD_WINDOWS.md](BUILD_WINDOWS.md) for the official Windows build
instructions. For quick validation without Visual Studio, MinGW-w64 on
Windows can build the ImGui backend (ScreenCapture remains disabled).

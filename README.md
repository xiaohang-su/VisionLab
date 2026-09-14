# VisionLab

A modular computer vision experimentation and runtime platform for Windows 10/11.

## Version

**V0.8.0 - UI Architecture Layer**

## Current Status

VisionLab V0.8.0 is released.

The current foundation includes:

- Core type system (Id, Timestamp, Frame, Detection, Track, AnalysisResult)
- Runtime lifecycle state machine
- Module system with lifecycle coordination
- Logger (file + console, thread-safe)
- Config system (INI format, minimal loading)
- Capture framework with pluggable CaptureSource architecture
- MockCaptureSource for testing
- Windows ScreenCaptureSource (Graphics Capture API)
- Vision Pipeline architecture (VisionProcessor + VisionPipeline + ImageView)
- Runtime frame loop connecting Capture → VisionPipeline
- Detection module architecture (Detector interface + DetectionResult)
- MockDetector for data flow validation
- Tracking module architecture (Tracker interface + TrackResult)
- MockTracker for data flow validation
- Analysis module architecture (Analyzer interface + AnalysisResult)
- MockAnalyzer for data flow validation
- UI architecture layer (UIRenderer interface + UIContext data binding)
- MockUI renderer for data flow validation

V0.8 does **not** include real window rendering, Dear ImGui, DirectX/Direct2D backend, or user interaction.

The project currently uses only the C++20 standard library and Windows SDK.
No third-party dependencies.

---

## Architecture

### Module Dependency Direction

```
Core (types, module interface)
  ← Runtime (lifecycle)
    ← Capture (input sources)
      ← Vision / Tracking / Analysis (future)
        ← UI (future)
```

### Key Principles

- **Simple** → **Modular** → **Testable** → **Extensible**
- No premature abstraction
- No third-party dependencies until necessary
- Platform-specific code isolated in implementation files

---

## Core Module

The Core module provides shared types and basic infrastructure.

### Id

- `uint64_t` based identifier
- Comparison operators
- Hash support
- Validity checking

### Timestamp

- `std::chrono::system_clock`
- Unix epoch semantics
- Millisecond precision
- Microsecond precision
- Comparison support
- Duration calculation

### Frame

- `Id` and `Timestamp`
- Width / height
- `PixelFormat` (BGRA8, BGR8, RGBA8, RGB8)
- `std::vector<uint8_t>` pixel data

### RuntimeState

Current lifecycle states:

```text
Created
Initialized
Running
Stopped
```

---

## Capture Module

The Capture module provides a pluggable source architecture.

### CaptureSource Interface

```cpp
class CaptureSource {
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool read(Frame& frame) = 0;
};
```

### Available Sources

| Source | Platform | Description |
|--------|----------|-------------|
| MockCaptureSource | All | Generates test frames (640x480 BGRA8 gradient) |
| ScreenCaptureSource | Windows 10 1903+ | Real screen capture via Graphics Capture API |

### Data Flow

```
CaptureSource::read(Frame&)
  ← fills width, height, format, data
Capture::get_frame(Frame&)
  ← sets frame.id, frame_count
FrameProvider interface
  ← consumed by Runtime / future Vision pipeline
```

---

## Build

### Requirements

- Windows 10 1903+ (for ScreenCaptureSource)
- CMake 3.20+
- C++20 compiler (MSVC 2022 recommended)
- Windows SDK

### Build Steps

```bash
cmake -B build
cmake --build build --config Release
```

### Linux Development

The platform-independent framework (Core, Runtime, Logger, Config, Capture interface, MockCaptureSource) can be syntax-checked on Linux:

```bash
g++ -std=c++20 -fsyntax-only -I src <file>.cpp
```

Windows-specific sources are excluded via `if(WIN32)` in CMake.

---

## Version History

| Version | Description |
|---------|-------------|
| V0.1 | Initial project skeleton |
| V0.2 | Foundation stable (Core, Runtime, Logger, Config, Module System) |
| V0.3.0 | Pluggable CaptureSource architecture + MockCaptureSource |
| V0.3.1 | Windows ScreenCaptureSource (Graphics Capture API) |
| V0.4.0 | Vision Pipeline architecture (VisionProcessor + VisionPipeline + ImageView) |
| V0.4.1 | Runtime frame loop connecting Capture → VisionPipeline |
| V0.5.0 | Detection module architecture (Detector + DetectionResult + MockDetector) |
| V0.6.0 | Tracking module architecture (Tracker + TrackResult + MockTracker) |
| V0.7.0 | Analysis module architecture (Analyzer + AnalysisResult + MockAnalyzer) |
| V0.8.0 | UI architecture layer (UIRenderer + UIContext + MockUI) |

---

## Roadmap

- **V0.8.x**: Real UI rendering (Dear ImGui + DirectX11, white minimal rounded style)
- **V1.0**: Full platform release

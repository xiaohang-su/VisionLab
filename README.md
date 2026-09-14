# VisionLab

A modular computer vision experimentation and runtime platform for Windows 10/11.

## Version

**V0.9.4 - Performance Dashboard**

## Current Status

VisionLab V0.9.4 is released.

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
- MockUI renderer for headless data flow validation
- Optional Dear ImGui + DirectX11 UI renderer
- Win32 window backend
- Frame viewer with detection/tracking overlays
- Analysis dashboard panel
- White minimal rounded UI style
- RuntimeMetrics with per-stage frame timing (capture/vision/detection/tracking/analysis/render)
- Timer utility (steady_clock, microsecond precision)
- FPS measurement and UI performance display
- Logger timing instrumentation (total_log_ms, log_count) for performance analysis

V0.9 introduces **runtime performance measurement**:

- Per-stage timing for each pipeline phase
- FPS calculation based on elapsed time
- RuntimeMetrics exposed via UIContext for display
- Both MockUI and ImGuiRenderer show performance metrics

V0.9.1 adds **logger performance measurement**:

- Logger internal timing (steady_clock, includes lock + format + console + file flush)
- RuntimeMetrics.logger_ms and RuntimeMetrics.log_count (cumulative)
- No behavior change: synchronous IO, per-call flush, original API preserved
- No async buffering, no thread, no queue introduced
- Purpose: measure whether Logger is a bottleneck before optimizing

V0.9.2 adds **frame lifetime measurement**:

- Frame data size tracking (frame_data_bytes)
- Vector allocation monitoring (frame_allocation_count)
- Data copy bytes tracking (frame_copy_bytes)
- Texture upload bytes and count monitoring (texture_upload_bytes, texture_upload_count)
- No behavior change: Frame API, Capture, VisionPipeline all unchanged
- No FramePool, no MemoryPool, no optimization introduced
- Purpose: measure frame memory cost before optimizing pipeline

V0.9.3 adds **performance attribution measurement**:

- Texture upload timing (texture_upload_ms): measures D3D11 Map + memcpy + Unmap only
- Vision copy bandwidth estimation (vision_copy_bandwidth_MBps)
- UIContext mutable feedback field for renderer-to-application metrics
- No behavior change: Vision, Capture, UIRenderer interfaces all unchanged
- No optimization code introduced
- Purpose: attribute time cost to specific data transfer operations

V0.9.4 adds **performance dashboard visualization**:

- Stage Timeline with per-stage progress bars (Capture/Vision/Detection/Tracking/Analysis/Render)
- Bottleneck Ranking (top 3 stages sorted by time, UI-local sorting)
- Warning indicators (green <30%, yellow 30-60%, red >60%)
- Memory Flow panel (frame size, copy total, upload total, upload time, bandwidth)
- UI-only changes: no RuntimeMetrics, Application, or core module modifications
- No Dashboard class, no PerformanceManager, no history storage
- Purpose: convert raw metrics into engineer-readable diagnostic information

V0.9 does **not** include Logger buffer optimization, FramePool, memory pool, benchmark system, AI Runtime, ONNX Runtime, CUDA, or TensorRT.

The core modules (Core, Runtime, Capture, Vision, Detection, Tracking, Analysis) remain zero third-party dependencies.

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
| V0.8.1 | ImGui DX11 UI renderer (Frame viewer + overlays + analysis panel) |
| V0.9.0 | Runtime metrics with per-stage frame timing (Timer + RuntimeMetrics + UI display) |
| V0.9.1 | Logger performance analysis (timing instrumentation, no behavior change) |
| V0.9.2 | Frame lifetime analysis (allocation, copy, texture upload measurement) |
| V0.9.3 | Performance attribution (texture upload timing + vision copy bandwidth) |
| V0.9.4 | Performance dashboard (stage timeline + bottleneck ranking + memory flow) |

---

## Roadmap

- **V0.9.5**: Windows/MSVC build verification
- **V1.0**: Stable platform release

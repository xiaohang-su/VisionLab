# VisionLab

A modular C++20 computer vision platform for Windows 10/11.

## Version

**V1.0 Stable Platform** (Release Candidate)

VisionLab is a stable vision computation platform skeleton with a complete
data pipeline: Capture → Vision → Detection → Tracking → Analysis → UI,
plus a runtime performance measurement system.

---

## Architecture

```
VisionLab
│
├── Core              — Id, Timestamp, Frame, PixelFormat, Module, Timer
├── Runtime           — Runtime lifecycle, RuntimeMetrics
├── Logging           — Synchronous logger with timing instrumentation
├── Config            — Minimal INI config loader
│
├── Capture           — Pluggable CaptureSource (Mock / Windows Screen Capture)
├── Vision            — VisionPipeline with ordered VisionProcessor chain
├── Detection         — Detector interface + DetectionResult
├── Tracking          — Tracker interface + TrackResult
├── Analysis          — Analyzer interface + AnalysisResult
│
└── UI                — UIRenderer interface
    ├── MockUI        — Headless stats output (zero dependencies)
    └── ImGuiRenderer — Dear ImGui + DirectX11 + Win32 (optional)
```

### Dependency Direction

```
Core
  ▲
  │
Runtime
  ▲
  │
Capture ← Vision ← Detection ← Tracking ← Analysis
  ▲
  │
UI (reads all above via const pointers, never writes back)
```

### Data Flow

```
CaptureSource::read(Frame&)
        ↓  Frame (original)
VisionProcessor::process(const Frame&, Frame&)
        ↓  Frame (processed)
Detector::detect(const Frame&, DetectionResult&)
        ↓  DetectionResult
Tracker::update(const DetectionResult&, TrackResult&)
        ↓  TrackResult
Analyzer::analyze(const DetectionResult&, const TrackResult&, AnalysisResult&)
        ↓  AnalysisResult
UIRenderer::render(const UIContext&)
```

All stages use const-reference input and reference output. No per-frame
allocation. Single-threaded frame loop.

---

## Features

### Pipeline Modules

| Module | Interface | Mock Implementation |
|--------|-----------|---------------------|
| Capture | `CaptureSource::read(Frame&)` | MockCaptureSource (640×480 BGRA gradient) |
| Vision | `VisionProcessor::process(const Frame&, Frame&)` | PassThroughProcessor |
| Detection | `Detector::detect(const Frame&, DetectionResult&)` | MockDetector (fixed center bbox) |
| Tracking | `Tracker::update(const DetectionResult&, TrackResult&)` | MockTracker (incremental track IDs) |
| Analysis | `Analyzer::analyze(const DetectionResult&, const TrackResult&, AnalysisResult&)` | MockAnalyzer |

### UI

- **UIRenderer** abstract interface — backend-agnostic
- **MockUI** — headless, prints stats to console, zero dependencies
- **ImGuiRenderer** — Dear ImGui + DirectX11, white minimal rounded style
  - Frame viewer with detection/tracking overlays
  - Performance dashboard (stage timeline, bottleneck ranking, memory flow)
  - FPS and per-stage timing display

### Runtime Performance System

V0.9 introduced a complete measurement framework:

- **Per-stage timing**: capture/vision/detection/tracking/analysis/render ms
- **Logger analysis**: log count, cumulative log time
- **Frame lifetime**: allocation count, copy bytes, texture upload bytes
- **Cost attribution**: texture upload ms, vision copy bandwidth
- **Performance dashboard**: engineer-readable visualization in ImGui UI

> **Principle**: measure first, optimize later. No FramePool, async logger,
> or multi-threading has been added without measured evidence of need.

---

## Build

### Requirements

- **OS**: Windows 10 (2004+) or Windows 11
- **Compiler**: Visual Studio 2022 (17.10+ recommended for `std::format`)
- **CMake**: 3.20+
- **C++ Standard**: C++20

### Default Build (zero dependencies)

```bash
cmake -B build -S .
cmake --build build --config Release
```

Produces `build/bin/Release/VisionLab.exe` with MockUI + MockCapture.
No third-party libraries required.

### With ImGui + DX11 UI

```bash
cmake -B build -S . -DVISIONLAB_UI_IMGUI=ON
cmake --build build --config Release
```

### With Windows Screen Capture

```bash
cmake -B build -S . -DVISIONLAB_UI_IMGUI=ON -DVISIONLAB_ENABLE_SCREEN_CAPTURE=ON
cmake --build build --config Release
```

> **Note**: `VISIONLAB_ENABLE_SCREEN_CAPTURE` requires MSVC + Windows SDK
> with C++/WinRT support. It is automatically disabled on MinGW/Clang.

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `VISIONLAB_UI_IMGUI` | OFF | Enable Dear ImGui + DirectX11 renderer |
| `VISIONLAB_ENABLE_SCREEN_CAPTURE` | OFF | Enable Windows Graphics Capture (MSVC only) |

### Linux Development

Linux is supported for syntax checking and architecture validation only.
See [docs/BUILD_LINUX.md](docs/BUILD_LINUX.md).

---

## Documentation

| Document | Purpose |
|----------|---------|
| [docs/API_FREEZE.md](docs/API_FREEZE.md) | V1.0 API freeze specification — what cannot change |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Architecture design rationale and module boundaries |
| [docs/BUILD_WINDOWS.md](docs/BUILD_WINDOWS.md) | Windows build guide and verification checklist |
| [docs/BUILD_LINUX.md](docs/BUILD_LINUX.md) | Linux development workflow |
| [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) | Common build and runtime issues |

---

## What V1.0 Does NOT Include

These are deliberately deferred — see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
for rationale:

- ❌ ONNX Runtime / CUDA / TensorRT / AI inference
- ❌ OpenCV
- ❌ Multi-threaded pipeline
- ❌ FramePool / memory recycling
- ❌ Async logger / buffered IO
- ❌ High DPI awareness
- ❌ Window resize / swap chain resize
- ❌ Plugin system
- ❌ Real detection/tracking algorithms (only mock implementations)

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
| V0.9.0 | Runtime metrics with per-stage frame timing |
| V0.9.1 | Logger performance analysis (timing instrumentation) |
| V0.9.2 | Frame lifetime analysis (allocation, copy, texture upload) |
| V0.9.3 | Performance attribution (upload timing + copy bandwidth) |
| V0.9.4 | Performance dashboard (stage timeline + bottleneck ranking) |
| V0.9.5 | Windows build verification (MinGW validation, 6 build bugs found) |
| V0.9.6 | Stabilization (CMake fixes, cross-platform, lifecycle, docs) |
| V1.0 | **Stable Platform** — API freeze, architecture documentation |

---

## Roadmap

```
v1.0  Stable Platform (current)
  │
  ├── v1.1  Runtime Refactor
  ├── v1.2  Vision Pipeline Expansion (Grayscale, Resize)
  ├── v1.3  ONNX Runtime Integration
  ├── v1.4  GPU Backend (CUDA / TensorRT)
  └── v2.0  AI Vision Platform
```

---

## License

See repository for license information.

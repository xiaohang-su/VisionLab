# VisionLab Architecture

> **Version**: V1.0 Stable Platform
> **Status**: Frozen architecture — no structural changes in V1.x releases.

VisionLab is a modular C++20 computer vision platform for Windows 10/11.
This document explains **why** the system is structured the way it is,
not just **what** each module does. For API-level contracts, see
[API_FREEZE.md](API_FREEZE.md).

---

## 1. Design Philosophy

VisionLab follows four principles, in priority order:

1. **Simple** — fewer abstractions beat clever abstractions.
2. **Modular** — each module has one responsibility and clear boundaries.
3. **Testable** — interfaces are pure virtual; implementations are swappable.
4. **Extensible** — new algorithms, backends, and UI layers can be added
   without modifying existing modules.

These principles override feature velocity. It is always acceptable to
defer a feature if it would require breaking a module boundary.

---

## 2. Module Overview

```
VisionLab
│
├── Core                 — Fundamental types and utilities
│   ├── types/           — Id, Timestamp, Frame, PixelFormat
│   ├── module.h         — Module lifecycle interface
│   ├── module_manager   — Runtime-managed module registry
│   ├── application      — Frame loop coordinator
│   └── timer            — High-resolution stopwatch
│
├── Runtime              — Runtime state and metrics
│   ├── runtime          — Runtime lifecycle
│   ├── runtime_context  — Shared runtime context
│   └── runtime_metrics  — Performance measurement data
│
├── Logging              — Logger (synchronous, leveled)
├── Config               — Minimal configuration loader
│
├── Capture              — Frame input
│   ├── capture          — Capture orchestrator
│   ├── capture_source   — Abstract input source
│   ├── mock_capture     — Deterministic test source
│   └── screen_capture   — Windows Graphics Capture (MSVC only)
│
├── Vision               — Image processing pipeline
│   ├── vision_processor — Abstract frame processor
│   ├── vision_pipeline  — Ordered processor chain (double-buffered)
│   ├── image_view       — Non-owning frame view
│   └── pass_through     — Identity processor (default)
│
├── Detection            — Object detection interface
│   ├── detector         — Abstract detector
│   ├── detection_result — Detection + BoundingBox data
│   └── mock_detector    — Fixed-result test detector
│
├── Tracking             — Multi-object tracking interface
│   ├── tracker          — Abstract tracker
│   ├── track_result     — Track data
│   └── mock_tracker     — ID-assigning test tracker
│
├── Analysis             — Pipeline statistics and analysis
│   ├── analyzer         — Abstract analyzer
│   ├── analysis_result  — Analysis output
│   └── mock_analyzer    — Test analyzer
│
└── UI                   — Display layer
    ├── ui_context       — Read-only data binding + UI feedback
    ├── ui_renderer      — Abstract renderer interface
    ├── ui               — UI coordinator
    ├── mock_ui          — Headless stats output
    └── backends/
        ├── win32_window     — Win32 window wrapper
        └── imgui_renderer   — Dear ImGui + DirectX11 renderer
```

---

## 3. Dependency Direction

Dependencies flow **downward only**. A lower module must never include
a header from a higher module.

```
Core (types, timer, module interface)
  ▲
  │
Runtime (metrics, lifecycle)
  ▲
  │
Capture  ←  Vision  ←  Detection  ←  Tracking  ←  Analysis
  ▲
  │
UI (reads from all above, never writes back)
```

**Key rules:**
- `Core` depends on nothing (standard library only).
- `Capture`, `Vision`, `Detection`, `Tracking`, `Analysis` are peers in
  the data pipeline but depend on `Core` and `Runtime`.
- `Detection` does **not** depend on `Vision` — it consumes `Frame` directly.
- `Tracking` does **not** depend on `Frame` — it consumes `DetectionResult`.
- `UI` depends on data types from all pipeline modules but never calls
  their methods. It only reads through `UIContext` pointers.
- No module depends on `Application`. `Application` is the top-level
  coordinator and is not included by any module.

---

## 4. Data Flow

The runtime frame loop (in `Application::run()`) executes:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Frame Loop                    │
│                                                             │
│  capture_.get_frame(original_frame_)                        │
│          │                                                  │
│          ▼                                                  │
│  vision_pipeline_.process(original_frame_, processed_frame_)│
│          │                                                  │
│          ▼                                                  │
│  detector_->detect(processed_frame_, detection_result_)     │
│          │                                                  │
│          ▼                                                  │
│  tracker_->update(detection_result_, track_result_)         │
│          │                                                  │
│          ▼                                                  │
│  analyzer_->analyze(detection_result_, track_result_,       │
│                     analysis_result_)                       │
│          │                                                  │
│          ▼                                                  │
│  ui_.render(ui_context_)  ← reads all above via pointers    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Design decisions:**
- **Reference output, not return value.** Every stage takes `const T&` input
  and writes to `T&` output. This avoids per-frame allocation and keeps
  ownership clear.
- **Original frame is immutable.** `VisionProcessor::process` takes
  `const Frame& input` — the original capture is never modified.
- **frame_id propagates downward.** `DetectionResult.frame_id` →
  `TrackResult.frame_id` → `AnalysisResult.frame_id`. Downstream stages
  never regenerate IDs.
- **Single-threaded.** The entire loop runs on one thread. No locks,
  no queues, no event bus. This is intentional for V1.0.

---

## 5. Why No Manager/Factory/Service Pattern

VisionLab deliberately avoids the common "Manager of Everything" pattern:

- **No `CaptureManager`** — `Application` holds one `Capture` directly.
- **No `DetectorFactory`** — detectors are created in `main()` and injected
  via `Application::set_detector(Detector*)`.
- **No `PerformanceManager`** — `RuntimeMetrics` is a plain struct; timing
  is done inline in `Application` with `Timer`.
- **No `EventBus`** — data flows through direct function calls in the loop.

**Rationale:** At V1.0 scale (one capture source, one pipeline, one detector,
one tracker, one analyzer, one renderer), indirection layers add complexity
without benefit. Managers become useful when there are multiple instances
or dynamic discovery — neither exists yet.

---

## 6. Why Detector/Tracker/Analyzer Don't Inherit Module

`Module` defines `initialize()/start()/stop()` for Runtime-managed
components with explicit lifecycle. The pipeline processors have simpler
needs:

- `VisionProcessor` is stateless — `process()` is its only method.
- `Detector` may load a model in its constructor, but doesn't need
  start/stop phases.
- `Tracker` maintains internal state but doesn't need Runtime coordination.
- `UIRenderer` has backend-specific lifecycle (`initialize`/`shutdown`)
  that doesn't match `Module`.

Forcing them into `Module` would require empty `start()/stop()` overrides
and couple them to the Runtime module. Keeping them independent is cleaner.

---

## 7. UI Architecture

UI is the **topmost layer** and follows strict rules:

### 7.1 UIContext is the only data channel

```cpp
struct UIContext {
    const Frame* frame;
    const DetectionResult* detections;
    const TrackResult* tracks;
    const AnalysisResult* analysis;
    const RuntimeMetrics* metrics;
    float fps;
    uint64_t frame_count;
    mutable double texture_upload_ms;  // UI → Application feedback
};
```

- All pipeline data is `const*` — UI cannot modify core data.
- `mutable texture_upload_ms` is the **only** write-back channel, used
  for UI-produced metrics (GPU upload time). This is a controlled
  exception, documented in API_FREEZE.md.

### 7.2 UIRenderer is backend-agnostic

```cpp
class UIRenderer {
    virtual bool render(const UIContext& context) = 0;
};
```

Two backends exist:
- **MockUI** — headless, prints stats to console. Zero dependencies.
- **ImGuiRenderer** — Dear ImGui + DirectX11 + Win32. Windows only.

Switching backends requires no changes to `Application`, `UIContext`,
or any pipeline module.

### 7.3 ImGui is isolated

Dear ImGui and DirectX11 exist **only** in:
- `third_party/imgui/`
- `src/ui/backends/imgui_renderer.cpp`
- `src/ui/backends/win32_window.cpp`

No core module includes `imgui.h` or `d3d11.h`. The CMake option
`VISIONLAB_UI_IMGUI=OFF` produces a build with zero third-party
dependencies.

---

## 8. Capture Architecture

### 8.1 CaptureSource is pluggable

```cpp
class CaptureSource {
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool read(Frame& frame) = 0;
};
```

`Capture` holds a non-owning `CaptureSource*`. The source is created in
`main()` and injected. This allows:
- `MockCaptureSource` for testing without a display
- `ScreenCaptureSource` for real Windows capture
- Future `CameraCaptureSource`, `VideoFileSource`, etc.

### 8.2 ScreenCapture uses pImpl

`screen_capture_source.h` contains zero Windows types:

```cpp
class ScreenCaptureSource : public CaptureSource {
    void* impl_ = nullptr;  // all Windows/COM/D3D11 state lives here
};
```

The implementation (`screen_capture_source.cpp`) includes `<windows.h>`,
`<d3d11.h>`, and WinRT headers. This keeps the public header cross-platform
and prevents Windows types from leaking into `Capture` or `Core`.

### 8.3 ScreenCapture is MSVC-only

Windows Graphics Capture (WinRT) requires the Windows SDK and C++/WinRT,
which MinGW does not ship. The CMake option
`VISIONLAB_ENABLE_SCREEN_CAPTURE` defaults to OFF and is gated on
`if(MSVC AND ...)`. MinGW builds automatically use `MockCaptureSource`.

---

## 9. Runtime Metrics Philosophy

V0.9 introduced `RuntimeMetrics` with a strict principle:

> **Measure first, optimize later. Never optimize without data.**

The metrics struct collects:
- Per-stage timing (capture/vision/detection/tracking/analysis/render ms)
- Frame lifetime (allocation count, copy bytes, upload bytes)
- Cost attribution (upload ms, copy bandwidth)
- Logger overhead (log count, log ms)

These are **measurement only**. V1.0 does not include FramePool, memory
recycling, async logging, or multi-threading — because there is no
measured evidence that any of these are needed.

When metrics show a real bottleneck, optimization targets that specific
stage rather than introducing a generic "performance system."

---

## 10. Build System Design

### 10.1 Zero-dependency default

```bash
cmake -B build -S .          # MockUI + MockCapture, no third-party libs
cmake --build build
```

The default build compiles only C++20 standard library code. This ensures:
- Core modules are always buildable on any C++20 compiler
- CI can run without Windows SDK or GPU drivers
- New contributors can build in minutes

### 10.2 Optional features via CMake options

| Option | Default | Enables |
|--------|---------|---------|
| `VISIONLAB_UI_IMGUI` | OFF | Dear ImGui + DX11 renderer |
| `VISIONLAB_ENABLE_SCREEN_CAPTURE` | OFF | Windows Graphics Capture (MSVC) |

Each option adds source files and link libraries only when enabled.
No optional code is compiled into the default build.

### 10.3 Windows guard

```cmake
if(NOT WIN32)
    message(FATAL_ERROR "VisionLab requires Windows 10/11")
endif()
```

VisionLab targets Windows exclusively (DirectX, Win32, Graphics Capture).
The guard prevents misleading partial builds on Linux. Linux is used for
development and syntax checking only — see [BUILD_LINUX.md](BUILD_LINUX.md).

---

## 11. What V1.0 Deliberately Excludes

These are **not bugs or missing features** — they are deferred by design:

| Capability | Why deferred |
|------------|-------------|
| ONNX Runtime / CUDA / TensorRT | AI inference requires model lifecycle, GPU memory management, async queue — too large for V1.0 |
| Multi-threaded pipeline | Single-threaded is simpler and sufficient at current scale |
| FramePool / memory recycling | No measured evidence of allocation bottleneck |
| Async logger | Logger overhead is measured; if it becomes significant, optimize then |
| High DPI awareness | Known limitation, tracked for V1.x |
| Window resize / swap chain resize | Known limitation, tracked for V1.x |
| Plugin system | No third-party plugin use case yet |
| Configuration UI | Config is file-based; no editor needed |

---

## 12. V1.x Evolution Path

```
v1.0  Stable Platform (this release)
  │
  ├── v1.1  Runtime Refactor
  │       └── Application split, FramePipeline extraction
  │
  ├── v1.2  Vision Pipeline Expansion
  │       └── Grayscale, Resize, color conversion processors
  │
  ├── v1.3  ONNX Runtime Integration
  │       └── Real detector backend, model loading, inference timing
  │
  ├── v1.4  GPU Backend
  │       └── CUDA/TensorRT, GPU frame transfer
  │
  └── v2.0  AI Vision Platform
          └── Multi-model, async inference, plugin system
```

Each step builds on the frozen V1.0 APIs. No V1.x release breaks
compatibility with V1.0 interfaces.

---

*Related documents: [API_FREEZE.md](API_FREEZE.md) · [BUILD_WINDOWS.md](BUILD_WINDOWS.md) · [BUILD_LINUX.md](BUILD_LINUX.md) · [TROUBLESHOOTING.md](TROUBLESHOOTING.md)*

# VisionLab V1.0 API Freeze Specification

> **Status**: V1.0 baseline — reviewed and corrected after freeze audit.
> **Effective**: V1.0.0 and all V1.x patch/minor releases.
> **Scope**: Core data structures, module interfaces, data flow contracts.

This document defines interfaces that **must not change** in V1.x releases without
a formal compatibility review. Breaking changes require a major version bump (V2.0).

---

## 1. Freeze Scope Overview

| Layer | Interface | Freeze Level | Status |
|-------|-----------|-------------|--------|
| Core | `Id` | 🔒 Strict | Stable |
| Core | `Timestamp` | 🔒 Strict | Stable |
| Core | `PixelFormat` | 🔒 Strict | Stable |
| Core | `Frame` | 🔒 Strict (CPU model only — see §2.1) | Stable |
| Core | `Module` | 🔒 Strict (Runtime-managed components only — see §3.1) | Stable |
| Runtime | `RuntimeMetrics` | ⚠️ Soft (append-only, split when growth requires) | 18 fields |
| Capture | `CaptureSource` | 🔒 Strict | Stable |
| Vision | `VisionProcessor` | 🔒 Strict | Stable |
| Detection | `Detector` | 🔒 Strict | Stable |
| Detection | `DetectionResult` / `Detection` / `BoundingBox` | 🔒 Strict | Stable |
| Tracking | `Tracker` | 🔒 Strict | Stable |
| Tracking | `TrackResult` / `Track` | 🔒 Strict | Stable |
| Analysis | `Analyzer` | 🔒 Strict | Stable |
| Analysis | `AnalysisResult` | 🔒 Strict | Stable |
| UI | `UIRenderer` | 🔒 Strict | Stable |
| UI | `UIContext` | ⚠️ Soft (append-only; mutable limited to UI feedback) | Stable |

**Not frozen**: `Application` (internal coordinator, may be refactored in V1.1),
all `Mock*` implementations, UI backend implementations, CMake options, documentation.

---

## 2. Core Data Structures (Strict Freeze)

### 2.1 Frame — CPU Memory Model

```cpp
namespace visionlab::core::types {

enum class PixelFormat {
    Unknown,
    BGRA8,
    BGR8,
    RGBA8,
    RGB8,
};

struct Frame {
    Id id;
    Timestamp timestamp;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    PixelFormat format = PixelFormat::Unknown;
    std::vector<std::uint8_t> data;
};

}
```

**Freeze rules:**
- Field order, names, and types are immutable.
- `PixelFormat`: existing numeric values must never change; new values append only.
- `data` remains `std::vector<std::uint8_t>` — do not replace with `span`,
  `unique_ptr`, or shared ownership types.

**CPU/GPU boundary (V1.0 clarification):**

> `Frame.data` represents **CPU-accessible frame storage** only.
> GPU memory representation (DirectX texture, CUDA device pointer, zero-copy
> buffers) is **outside the Frame contract**.
>
> Future GPU integration must use an **extension layer** — e.g. `GpuFrame`,
> `FrameView`, `FrameBuffer` — and must not modify `Frame` itself.
> The CPU `Frame` remains the canonical interchange format between Capture,
> Vision, Detection, Tracking, and Analysis.

### 2.2 Id

```cpp
namespace visionlab::core::types {
class Id {
public:
    using ValueType = std::uint64_t;
    explicit Id(ValueType value = 0);
    ValueType value() const;
    bool is_valid() const;
    // comparison: == != < <= > >=
};
}
// std::hash<Id> specialization
```

**Freeze rules:** `ValueType` is immutable; comparison operators must not be removed;
new methods may be added but signatures must not change.

### 2.3 Detection Data Structures

```cpp
struct BoundingBox { float x, y, width, height; };

struct Detection {
    BoundingBox bbox;
    int class_id;
    float confidence;
};

struct DetectionResult {
    core::types::Id frame_id;
    std::vector<Detection> detections;
};
```

### 2.4 Tracking Data Structures

```cpp
struct Track {
    int id;
    BoundingBox bbox;
    int class_id;
    float confidence;
};

struct TrackResult {
    core::types::Id frame_id;
    std::vector<Track> tracks;
};
```

### 2.5 Identifier Type Note (V1.x migration candidate)

> `Detection.class_id` and `Track.id` currently use `int`.
> `core::Id` (`uint64_t`) exists for frame-level identity.
>
> This is recorded as a **V1.x migration candidate**: if AI multi-target scenarios
> require larger identifier space, consider replacing `int` with `core::Id` in a
> future minor release. **Do not change in V1.0** — it would break compatibility.

---

## 3. Core Interfaces (Strict Freeze)

### 3.1 Module — Runtime-Managed Lifecycle

```cpp
class Module {
public:
    virtual ~Module() = default;
    virtual const char* name() const = 0;
    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
};
```

**Scope clarification (V1.0 audit correction):**

> The `Module` lifecycle (`initialize` / `start` / `stop`) applies **only to
> Runtime-managed components**. It is **not** a universal base for all modules.
>
> The following do **not** inherit `Module` and have their own minimal
> lifecycle:
> - `CaptureSource` — `open()` / `close()` / `read()`
> - `VisionProcessor` — stateless `process()`
> - `Detector` — stateless `detect()`
> - `Tracker` — `update()` with internal state
> - `Analyzer` — stateless `analyze()`
> - `UIRenderer` — `initialize()` / `render()` / `shutdown()` (backend-specific)
>
> Do not force these to inherit `Module`. Their independence is intentional.

**Freeze rules:** the four-phase lifecycle is immutable; no new pure virtual
methods may be added.

### 3.2 CaptureSource

```cpp
class CaptureSource {
public:
    virtual ~CaptureSource() = default;
    virtual const char* name() const = 0;
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool read(core::types::Frame& frame) = 0;
};
```

**Freeze rules:** `read(Frame&)` output-by-reference is immutable; do not change
to return-by-value.

### 3.3 VisionProcessor

```cpp
class VisionProcessor {
public:
    virtual ~VisionProcessor() = default;
    virtual const char* name() const = 0;
    virtual bool process(const core::types::Frame& input,
                         core::types::Frame& output) = 0;
};
```

**Freeze rules:** const-input + mutable-output pattern is immutable; the original
frame must never be modified by a processor.

### 3.4 Detector

```cpp
class Detector {
public:
    virtual ~Detector() = default;
    virtual const char* name() const = 0;
    virtual bool detect(const core::types::Frame& frame,
                        DetectionResult& result) = 0;
};
```

### 3.5 Tracker

```cpp
class Tracker {
public:
    virtual ~Tracker() = default;
    virtual const char* name() const = 0;
    virtual bool update(const detection::DetectionResult& detections,
                        TrackResult& result) = 0;
};
```

**Freeze rules:** Tracker consumes `DetectionResult` only — it does not receive
`Frame`. `TrackResult.frame_id` is inherited from `DetectionResult.frame_id`,
not regenerated.

### 3.6 Analyzer

```cpp
class Analyzer {
public:
    virtual ~Analyzer() = default;
    virtual const char* name() const = 0;
    virtual bool analyze(const detection::DetectionResult& detections,
                         const tracking::TrackResult& tracks,
                         AnalysisResult& result) = 0;
};
```

### 3.7 UIRenderer

```cpp
class UIRenderer {
public:
    virtual ~UIRenderer() = default;
    virtual const char* name() const = 0;
    virtual bool render(const UIContext& context) = 0;
};
```

**Freeze rules:** `render(const UIContext&)` signature is immutable. `UIRenderer`
does not inherit `Module`.

---

## 4. Soft-Freeze Interfaces

### 4.1 RuntimeMetrics (Soft Freeze)

Current fields (18), grouped:

| Group | Fields |
|-------|--------|
| Timing | `fps`, `capture_ms`, `vision_ms`, `detection_ms`, `tracking_ms`, `analysis_ms`, `render_ms`, `logger_ms` |
| Counts | `frame_count`, `log_count` |
| Frame Lifetime | `frame_data_bytes`, `frame_allocation_count`, `frame_copy_bytes`, `texture_upload_bytes`, `texture_upload_count` |
| Cost Attribution | `texture_upload_ms`, `vision_copy_bandwidth_MBps` |

**Freeze rules:**
- ✅ Allowed: append new fields at the end with default values.
- ❌ Forbidden: delete, rename, retype, or reorder existing fields.
- ⚠️ **Growth policy**: if field count grows significantly, the structure **must
  be split** into specialized snapshot sub-structs (e.g. `TimingSnapshot`,
  `MemorySnapshot`, `GpuSnapshot`). `RuntimeMetrics` should remain the **core
  runtime state** aggregate, not an unbounded dump. There is no fixed numeric
  threshold — the decision is architectural, driven by whether fields form a
  coherent sub-group.

### 4.2 UIContext (Soft Freeze)

```cpp
struct UIContext {
    const core::types::Frame* frame = nullptr;
    const detection::DetectionResult* detections = nullptr;
    const tracking::TrackResult* tracks = nullptr;
    const analysis::AnalysisResult* analysis = nullptr;
    const runtime::RuntimeMetrics* metrics = nullptr;

    // Convenience duplicates (written by Application each frame)
    float fps = 0.0f;
    std::uint64_t frame_count = 0;

    // UI feedback (V0.9.3): written by renderer during render()
    mutable double texture_upload_ms = 0.0;
};
```

**Freeze rules:**
- ✅ Allowed: add new read-only pointer fields.
- ✅ Allowed: add new `mutable` fields **only** for data produced by the UI
  backend itself (e.g. upload timing, draw call count).
- ❌ Forbidden: use `mutable` for non-UI-feedback purposes.
- ⚠️ **Evolution path (recorded, not implemented in V1.0)**: if `mutable`
  feedback fields exceed a small number (2–3), split into a separate
  `UIFeedback` structure:

  ```cpp
  // Future direction (V1.x, not V1.0):
  struct UIFeedback {
      double texture_upload_ms;
      // ... more UI-produced metrics
  };

  struct UIContext {
      // read-only data pointers ...
      UIFeedback* feedback = nullptr;  // written by renderer, read by app
  };
  ```

  This preserves `render(const UIContext&)` as logically read-only while
  allowing UI-to-application feedback without `mutable` abuse.

---

## 5. Data Flow Contract (Frozen)

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

**Freeze rules:**
- Data flows one direction only — no reverse dependencies in the pipeline.
- Every stage takes const-reference input and writes to reference output.
- `frame_id` propagates `DetectionResult → TrackResult → AnalysisResult`;
  it is never regenerated by a downstream stage.
- No Manager, Service, EventBus, or thread pool may be inserted into this
  data flow in V1.x.

---

## 6. Change Management

### 6.1 Version Semantics

| Version | Allowed changes |
|---------|----------------|
| **V1.0.x** (patch) | Bug fixes only. No frozen API changes. |
| **V1.x.0** (minor) | Append fields to soft-freeze structs; add optional interfaces; new backends. No breaking changes. |
| **V2.0.0** (major) | Breaking changes allowed, requiring one minor release of deprecation warnings first. |

### 6.2 Change Approval

Any modification to a strictly-frozen API must:
1. Produce an impact analysis (which modules and downstream consumers are affected).
2. Provide a compatibility path (deprecation period or adapter layer).
3. Receive explicit authorization before implementation.

### 6.3 Prohibited in V1.x

- ❌ Changing `Frame` field types or order.
- ❌ Replacing reference-passing with return-by-value or smart pointers.
- ❌ Adding pure virtual methods to frozen interfaces.
- ❌ Forcing `Detector`/`Tracker`/`Analyzer`/`UIRenderer` to inherit `Module`.
- ❌ Introducing third-party libraries (OpenCV, ONNX, CUDA) into core modules.
- ❌ Adding multi-threading, EventBus, or Manager classes into the data flow.

---

## 7. V1.0 Release Criteria

| # | Criterion | Status |
|---|-----------|--------|
| 1 | All frozen APIs in this document match source code | ✅ Verified |
| 2 | Windows MSVC Release Build passes | ⏳ Pending |
| 3 | ImGui ON/OFF both runnable | ⏳ MSVC pending (MinGW verified) |
| 4 | README updated to V1.0 | ⏳ Pending |
| 5 | `docs/ARCHITECTURE.md` added | ⏳ Pending |
| 6 | Known issues list complete | ⏳ Pending |
| 7 | `v1.0.0` tag created and pushed | ⏳ Pending |

---

## 8. V1.0 Implementation Plan

```
Step 1:  API Freeze Specification (this document)  ✅
   ↓
Step 2:  Windows MSVC Verification
   ↓
Step 3:  Documentation (ARCHITECTURE.md, README update, known issues)
   ↓
Step 4:  v1.0.0 Release (commit + tag + push)
```

**V1.0 adds no new features.** It is a stabilization and documentation release.

---

*Document version: 1.1 (post-audit corrections applied)*
*Audit corrections: Frame GPU boundary, RuntimeMetrics growth policy,
UIContext/UIFeedback evolution, Module lifecycle scope, ID type migration note.*

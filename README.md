# VisionLab

A modular computer vision experimentation and runtime platform for Windows 10/11.

## Version

**V0.2 Phase 1** - Core and Runtime Foundation

## Current Status

Core type system and minimal Runtime lifecycle implementation.

## Current Implementation

### Core Module

- **ID** - Simple identifier (uint64_t)
  - Copyable and comparable
  - Hash support
  - No external dependencies

- **Timestamp** - Unified time representation
  - std::chrono::system_clock
  - Unix epoch semantics
  - Millisecond and microsecond precision
  - Comparison and duration support

- **RuntimeState** - Lifecycle states
  - Created
  - Initialized
  - Running
  - Stopped

### Runtime Module

Minimal lifecycle manager:

- initialize() - Created → Initialized
- start() - Initialized → Running
- stop() - Running → Stopped
- state() - Query current state
- is_running() - Check if running

State validation prevents invalid transitions.

## Runtime Lifecycle

```
Created → Initialized → Running → Stopped
```

## Building

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Running

```bash
build/bin/VisionLab.exe
```

Expected output:

```
VisionLab V0.2
Runtime initialized.
Runtime started.
Runtime stopped.
```

## Project Structure

```
src/
├── main.cpp
├── core/
│   ├── core_types.h
│   └── types/
│       ├── id.h
│       ├── timestamp.h
│       └── runtime_state.h
└── runtime/
    ├── runtime.h
    └── runtime.cpp
```

## Not Yet Implemented

- Capture, Vision, Tracking, Analysis, UI modules
- Logger and Config systems
- Any third-party dependencies

## Third-Party Dependencies

**Current:** None (C++20 standard library only)

## Author

VisionLab Development Team

# VisionLab

A modular computer vision experimentation and runtime platform for Windows 10/11.

## Version

**V0.2 Foundation**

## Current Status

VisionLab V0.2 foundation is under development.

The current foundation includes:

- Core type system
- Runtime lifecycle
- Logger
- Config system

The project currently uses only the C++20 standard library and has no third-party dependencies.

---

## Core Module

The Core module provides shared types and basic infrastructure.

Current types include:

### ID

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

### RuntimeState

Current lifecycle states:

```text
Created
Initialized
Running
Stopped
# VisionLab

A modular computer vision experimentation and runtime platform for Windows 10/11.

## Version

**V0.1** - Project Skeleton

## Current Status

Engineering skeleton with base directory structure and minimal entry point.

## Current Implementation

- C++20
- CMake build system
- Windows 10/11 support
- Minimal runnable program
- Base module directory structure

## Not Yet Implemented

- Capture module functionality
- Vision module functionality
- Detection system
- Tracking system
- Analysis module
- AI model inference
- OpenCV integration
- ONNX Runtime integration
- CUDA support
- TensorRT support
- DirectML support
- Dear ImGui integration
- DirectX 11 UI

## Architecture

VisionLab follows a modular architecture with clear separation of concerns:

```
Core (Base infrastructure)
├── Capture (Input layer)
├── Vision (Visual processing)
├── Runtime (Orchestration)
├── Tracking (Target tracking)
├── Analysis (Data analysis)
└── UI (User interface)
```

Each module is designed for low coupling and high cohesion, following the dependency principle:

```
Input → Capture → Frame → Vision → Detection → Tracking → Track → Analysis → Result → UI
```

Runtime coordinates the entire pipeline. Core provides base infrastructure.

## Building

### Prerequisites

- Windows 10/11
- CMake 3.20 or later
- Visual Studio 2019/2022 or MSVC compiler
- C++20 support

### Build Steps

```bash
# Configure the project
cmake -S . -B build

# Build the project (Release configuration)
cmake --build build --config Release

# Or Debug configuration
cmake --build build --config Debug
```

### Output

After successful build, the executable will be located at:

```
build/bin/VisionLab.exe
```

## Running

```bash
# Navigate to build directory
cd build/bin

# Run VisionLab
./VisionLab.exe
```

Expected output:

```
VisionLab V0.1
Project skeleton initialized.
```

## Project Structure

```
VisionLab/
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── .gitignore              # Git ignore rules
│
├── src/
│   ├── main.cpp            # Program entry point
│   ├── core/               # Base infrastructure (future)
│   ├── capture/            # Input layer (future)
│   ├── vision/             # Visual processing (future)
│   ├── runtime/            # Orchestration (future)
│   ├── tracking/           # Target tracking (future)
│   ├── analysis/           # Data analysis (future)
│   └── ui/                 # User interface (future)
│
├── models/                 # AI models (future)
├── config/                 # Configuration files (future)
├── assets/                 # Static resources (future)
├── logs/                   # Runtime logs (future)
└── third_party/            # External dependencies (future)
```

## Development Principles

1. **No Architecture Changes** - Respect the defined 7-module structure
2. **Keep main.cpp Simple** - Business logic belongs in modules, not main
3. **Low Coupling** - Modules communicate through clear interfaces
4. **Correct Module Placement** - Each feature goes in its designated module
5. **Avoid Circular Dependencies** - Follow the dependency direction
6. **No Premature Implementation** - Only implement current version features
7. **Lazy Dependency Loading** - Add third-party libraries only when needed
8. **Interface First** - Design for future extensibility
9. **Always Runnable** - Each version must compile and run

## Next Steps

V0.1 provides the foundation. The next phases will progressively add:

- **V0.2** - Core and Runtime infrastructure
- **V0.3** - Capture module and frame pipeline
- **V0.4** - Vision module and image processing
- **V0.5** - AI runtime and model inference
- **V0.6** - Tracking system
- **V0.7** - Analysis module
- **V0.8** - UI with Dear ImGui and DirectX 11
- **V1.0** - Complete integrated platform

## Third-Party Dependencies

**Current:** None

Future versions will integrate:
- OpenCV (image processing)
- ONNX Runtime (model inference)
- CUDA/TensorRT/DirectML (GPU acceleration)
- Dear ImGui (UI framework)
- DirectX 11 (graphics)

## License

(To be defined)

## Author

VisionLab Development Team

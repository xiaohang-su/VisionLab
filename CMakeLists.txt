cmake_minimum_required(VERSION 3.20)
project(VisionLab LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform requirement: Windows 10/11
if(NOT WIN32)
    message(FATAL_ERROR "VisionLab requires Windows 10/11")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Core source files
set(CORE_SOURCES
    src/core/types/id.h
    src/core/types/timestamp.h
    src/core/types/runtime_state.h
    src/core/core_types.h
)

# Runtime source files
set(RUNTIME_SOURCES
    src/runtime/runtime.h
    src/runtime/runtime.cpp
)

# Main executable
add_executable(VisionLab
    src/main.cpp
    ${CORE_SOURCES}
    ${RUNTIME_SOURCES}
)

# Set target properties
set_target_properties(VisionLab PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

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

# Main executable
add_executable(VisionLab src/main.cpp)

# Set target properties
set_target_properties(VisionLab PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# C++20 features
if(MSVC)
    target_compile_options(VisionLab PRIVATE /std:c++latest)
else()
    target_compile_options(VisionLab PRIVATE -std=c++20)
endif()

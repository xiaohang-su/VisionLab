cmake_minimum_required(VERSION 3.20)

project(VisionLab LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT WIN32)
    message(FATAL_ERROR "VisionLab requires Windows 10/11")
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")



# =========================
# Core
# =========================

set(CORE_SOURCES

    src/core/types/id.h
    src/core/types/timestamp.h
    src/core/types/runtime_state.h

    src/core/types/frame.h
    src/core/types/detection.h
    src/core/types/track.h
    src/core/types/analysis_result.h

    src/core/core_types.h

    src/core/module.h
    src/core/module_manager.h
    src/core/module_manager.cpp

)



# =========================
# Application
# =========================

set(APPLICATION_SOURCES

    src/core/application.h
    src/core/application.cpp

)



# =========================
# Runtime
# =========================

set(RUNTIME_SOURCES

    src/runtime/runtime.h
    src/runtime/runtime.cpp

    src/runtime/runtime_context.h
    src/runtime/runtime_context.cpp

)



# =========================
# Logging
# =========================

set(LOGGING_SOURCES

    src/logging/logger.h
    src/logging/logger.cpp

)



# =========================
# Config
# =========================

set(CONFIG_SOURCES

    src/config/config.h
    src/config/config.cpp

)



# =========================
# Capture
# =========================

set(CAPTURE_SOURCES

    src/capture/capture.h
    src/capture/capture.cpp

)



# =========================
# Executable
# =========================

add_executable(VisionLab

    src/main.cpp

    ${CORE_SOURCES}

    ${APPLICATION_SOURCES}

    ${RUNTIME_SOURCES}

    ${LOGGING_SOURCES}

    ${CONFIG_SOURCES}

    ${CAPTURE_SOURCES}

)



# =========================
# Visual Studio Debug
# =========================

set_target_properties(VisionLab PROPERTIES

    VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"

)
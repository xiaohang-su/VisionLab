cmake_minimum_required(VERSION 3.20)

project(VisionLab VERSION 1.1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static-link MSVC runtime (/MT Release, /MTd Debug)
# so the EXE needs no VC++ Redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


option(VISIONLAB_UI_IMGUI "Enable ImGui DX11 UI backend" OFF)
option(VISIONLAB_ENABLE_SCREEN_CAPTURE "Enable Windows Graphics Capture (MSVC only)" OFF)


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

    src/core/timer.h

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

    src/runtime/runtime_metrics.h

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

    src/capture/frame_provider.h
    src/capture/capture_source.h

    src/capture/mock_capture_source.h
    src/capture/mock_capture_source.cpp

)

# V0.9.6: Windows Graphics Capture requires MSVC + Windows SDK (C++/WinRT)
if(MSVC AND VISIONLAB_ENABLE_SCREEN_CAPTURE)
    list(APPEND CAPTURE_SOURCES

        src/capture/screen_capture_source.h
        src/capture/screen_capture_source.cpp

    )
    add_compile_definitions(VISIONLAB_ENABLE_SCREEN_CAPTURE)
endif()



# =========================
# Vision
# =========================

set(VISION_SOURCES

    src/vision/vision_processor.h
    src/vision/image_view.h
    src/vision/vision_pipeline.h
    src/vision/vision_pipeline.cpp
    src/vision/pass_through_processor.h
    src/vision/pass_through_processor.cpp

)



# =========================
# Detection
# =========================

set(DETECTION_SOURCES

    src/detection/detection_types.h
    src/detection/detection_result.h
    src/detection/detector.h
    src/detection/inference_detector.h
    src/detection/mock_detector.h
    src/detection/mock_detector.cpp

)



# =========================
# Inference (V1.1)
# =========================

set(INFERENCE_SOURCES

    src/inference/model_info.h
    src/inference/inference_backend.h
    src/inference/mock_inference_backend.h

)



# =========================
# Auth (V1.1)
# =========================

set(AUTH_SOURCES

    src/auth/auth_types.h
    src/auth/auth_service.h
    src/auth/mock_auth_service.h

)



# =========================
# Platform (V1.1)
# =========================

set(PLATFORM_SOURCES

    src/platform/platform_types.h
    src/platform/platform_service.h
    src/platform/mock_platform_service.h

)



# =========================
# Tracking
# =========================

set(TRACKING_SOURCES

    src/tracking/track_types.h
    src/tracking/track_result.h
    src/tracking/tracker.h
    src/tracking/mock_tracker.h
    src/tracking/mock_tracker.cpp

)



# =========================
# Analysis
# =========================

set(ANALYSIS_SOURCES

    src/analysis/analysis_types.h
    src/analysis/analysis_result.h
    src/analysis/analyzer.h
    src/analysis/mock_analyzer.h
    src/analysis/mock_analyzer.cpp

)



# =========================
# UI
# =========================

set(UI_SOURCES

    src/ui/ui_context.h
    src/ui/ui_renderer.h
    src/ui/ui.h
    src/ui/ui.cpp
    src/ui/mock_ui.h
    src/ui/mock_ui.cpp

)



# =========================
# ImGui UI Backend (optional)
# =========================

set(IMGUI_SOURCES)

set(UI_BACKEND_SOURCES)

if(VISIONLAB_UI_IMGUI)

    message(STATUS "VisionLab: ImGui DX11 UI backend enabled")

    add_compile_definitions(VISIONLAB_HAS_IMGUI)

    set(IMGUI_SOURCES

        third_party/imgui/imgui.h
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_internal.h
        third_party/imgui/imconfig.h
        third_party/imgui/imstb_rectpack.h
        third_party/imgui/imstb_textedit.h
        third_party/imgui/imstb_truetype.h

        third_party/imgui/backends/imgui_impl_dx11.h
        third_party/imgui/backends/imgui_impl_dx11.cpp
        third_party/imgui/backends/imgui_impl_win32.h
        third_party/imgui/backends/imgui_impl_win32.cpp

    )

    set(UI_BACKEND_SOURCES

        src/ui/backends/win32_window.h
        src/ui/backends/win32_window.cpp
        src/ui/backends/imgui_renderer.h
        src/ui/backends/imgui_renderer.cpp

        src/ui/backends/floating_window.h
        src/ui/backends/floating_window.cpp

    )

endif()



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

    ${VISION_SOURCES}

    ${DETECTION_SOURCES}

    ${INFERENCE_SOURCES}

    ${AUTH_SOURCES}

    ${PLATFORM_SOURCES}

    ${TRACKING_SOURCES}

    ${ANALYSIS_SOURCES}

    ${UI_SOURCES}

    ${IMGUI_SOURCES}

    ${UI_BACKEND_SOURCES}

)


# V0.9.6: add src/ as include root (was previously relying on manual -I src)
target_include_directories(VisionLab PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)


if(WIN32)
    target_link_libraries(VisionLab PRIVATE

        d3d11.lib
        dxgi.lib
        # V0.9.6: ImGui DX11 backend needs D3DCompile (shader compilation)
        d3dcompiler.lib
        # V0.9.6: ImGui Win32 backend uses DWM for blur/colorization
        dwmapi.lib
        windowsapp.lib

    )
endif()


if(VISIONLAB_UI_IMGUI)
    target_include_directories(VisionLab PRIVATE
        third_party/imgui
        third_party/imgui/backends
    )
endif()


# =========================
# Visual Studio Debug
# =========================

set_target_properties(VisionLab PROPERTIES

    VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"

)
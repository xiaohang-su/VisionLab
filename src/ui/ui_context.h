#pragma once

#include "../core/core_types.h"
#include "../detection/detection_result.h"
#include "../tracking/track_result.h"
#include "../analysis/analysis_result.h"
#include "../runtime/runtime_metrics.h"
#include "../auth/auth_types.h"
#include "../platform/platform_types.h"

#include <cstdint>
#include <vector>


namespace visionlab::ui {


struct UIContext
{

    const core::types::Frame* frame = nullptr;

    const detection::DetectionResult* detections = nullptr;

    const tracking::TrackResult* tracks = nullptr;

    const analysis::AnalysisResult* analysis = nullptr;

    const runtime::RuntimeMetrics* metrics = nullptr;


    float fps = 0.0f;

    std::uint64_t frame_count = 0;

    // UI feedback (V0.9.3): written by renderer during render()
    mutable double texture_upload_ms = 0.0;


    // === V1.1 Platform fields ===

    // Auth session (nullptr = not logged in)
    const auth::UserSession* session = nullptr;

    // Platform info (version, official group, server status)
    const platform::PlatformInfo* platform_info = nullptr;

    // Current weather
    const platform::WeatherInfo* weather = nullptr;

    // Announcement list
    const std::vector<platform::Announcement>* announcements = nullptr;

    // Current time string (HH:MM:SS)
    const char* time_string = nullptr;

};


}

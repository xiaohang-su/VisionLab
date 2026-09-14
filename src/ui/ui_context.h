#pragma once

#include "../core/core_types.h"
#include "../detection/detection_result.h"
#include "../tracking/track_result.h"
#include "../analysis/analysis_result.h"
#include "../runtime/runtime_metrics.h"

#include <cstdint>


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

};


}

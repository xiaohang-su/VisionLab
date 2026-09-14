#pragma once

#include "../core/core_types.h"
#include "../detection/detection_result.h"
#include "../tracking/track_result.h"
#include "../analysis/analysis_result.h"

#include <cstdint>


namespace visionlab::ui {


struct UIContext
{

    const core::types::Frame* frame = nullptr;

    const detection::DetectionResult* detections = nullptr;

    const tracking::TrackResult* tracks = nullptr;

    const analysis::AnalysisResult* analysis = nullptr;


    float fps = 0.0f;

    std::uint64_t frame_count = 0;

};


}

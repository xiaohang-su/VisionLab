#pragma once

#include "analysis_types.h"

#include "../core/types/id.h"

#include <vector>


namespace visionlab::analysis {


struct AnalysisResult
{

    core::types::Id frame_id;

    int detection_count = 0;

    int active_track_count = 0;

    std::vector<ClassStat> class_stats;

    double processing_time_ms = 0.0;

};


}

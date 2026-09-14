#pragma once

#include <cstdint>


namespace visionlab::runtime {


struct RuntimeMetrics
{

    double fps = 0.0;

    double capture_ms = 0.0;
    double vision_ms = 0.0;
    double detection_ms = 0.0;
    double tracking_ms = 0.0;
    double analysis_ms = 0.0;
    double render_ms = 0.0;
    double logger_ms = 0.0;

    std::uint64_t frame_count = 0;
    std::uint64_t log_count = 0;

};


}

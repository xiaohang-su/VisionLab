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

    // Frame lifetime (V0.9.2: measurement only)
    std::uint64_t frame_data_bytes = 0;
    std::uint64_t frame_allocation_count = 0;
    std::uint64_t frame_copy_bytes = 0;
    std::uint64_t texture_upload_bytes = 0;
    std::uint64_t texture_upload_count = 0;

    // Performance attribution (V0.9.3: measurement only)
    double texture_upload_ms = 0.0;
    double vision_copy_bandwidth_MBps = 0.0;

};


}

#pragma once

#include "id.h"
#include "timestamp.h"

#include <cstdint>

namespace visionlab::core::types {

struct AnalysisResult
{
    Id id;
    Timestamp timestamp;

    std::uint32_t object_count = 0;

    double processing_time_ms = 0.0;
};

}
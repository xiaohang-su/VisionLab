#pragma once

#include "id.h"
#include "timestamp.h"

#include <cstdint>

namespace visionlab::core::types {

struct Track
{
    Id id;
    Timestamp timestamp;

    std::uint32_t detection_count = 0;

    bool active = false;
};

}
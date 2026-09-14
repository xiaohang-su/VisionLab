#pragma once

#include "id.h"
#include "timestamp.h"

#include <cstdint>

namespace visionlab::core::types {

struct Frame {
    Id id;
    Timestamp timestamp;

    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

}
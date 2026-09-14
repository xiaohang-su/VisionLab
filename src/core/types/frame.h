#pragma once

#include "id.h"
#include "timestamp.h"

#include <cstdint>
#include <vector>

namespace visionlab::core::types {

enum class PixelFormat {
    Unknown,
    BGRA8,
    BGR8,
    RGBA8,
    RGB8,
};

struct Frame {
    Id id;
    Timestamp timestamp;

    std::uint32_t width = 0;
    std::uint32_t height = 0;

    PixelFormat format = PixelFormat::Unknown;

    std::vector<std::uint8_t> data;
};

}

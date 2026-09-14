#pragma once

#include "id.h"

#include <cstdint>

namespace visionlab::core::types {

struct Detection
{
    Id id;

    float x = 0.0f;
    float y = 0.0f;

    float width = 0.0f;
    float height = 0.0f;

    float confidence = 0.0f;

    std::uint32_t class_id = 0;
};

}
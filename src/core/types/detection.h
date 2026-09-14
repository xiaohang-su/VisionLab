#pragma once

#include "id.h"

#include <cstdint>

namespace visionlab::core::types {

struct BoundingBox {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

struct Detection {
    Id id;

    std::int32_t class_id = -1;
    float confidence = 0.0f;

    BoundingBox bounding_box;
};

}
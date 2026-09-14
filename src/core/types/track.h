#pragma once

#include "detection.h"

namespace visionlab::core::types {

enum class TrackState {
    Active,
    Lost
};

struct Track {
    Id id;

    std::int32_t class_id = -1;
    float confidence = 0.0f;

    BoundingBox bounding_box;

    TrackState state = TrackState::Active;
};

}
#pragma once

#include "detection_types.h"

#include "../core/types/id.h"

#include <vector>


namespace visionlab::detection {


struct DetectionResult
{

    core::types::Id frame_id;

    std::vector<Detection> detections;

};


}

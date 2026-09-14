#pragma once

#include "../detection/detection_types.h"


namespace visionlab::tracking {


struct Track
{

    int id = -1;

    detection::BoundingBox bbox;

    int class_id = -1;

    float confidence = 0.0f;

    bool active = true;

};


}

#pragma once

#include <cstdint>


namespace visionlab::detection {


struct BoundingBox
{

    float x = 0.0f;

    float y = 0.0f;

    float width = 0.0f;

    float height = 0.0f;

};


struct Detection
{

    BoundingBox bbox;

    int class_id = -1;

    float confidence = 0.0f;

};


}

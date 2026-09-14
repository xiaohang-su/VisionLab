#pragma once

#include "detection_result.h"

#include "../core/core_types.h"


namespace visionlab::detection {


class Detector
{

public:

    virtual ~Detector() = default;


    virtual const char* name() const = 0;


    virtual bool detect(
        const core::types::Frame& frame,
        DetectionResult& result
    ) = 0;

};


}

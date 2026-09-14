#pragma once

#include "detector.h"


namespace visionlab::detection {


class MockDetector
    : public Detector
{

public:

    const char* name() const override;


    bool detect(
        const core::types::Frame& frame,
        DetectionResult& result
    ) override;

};


}

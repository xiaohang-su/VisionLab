#pragma once

#include "vision_processor.h"


namespace visionlab::vision {


class PassThroughProcessor
    : public VisionProcessor
{

public:

    const char* name() const override;


    bool process(
        const core::types::Frame& input,
        core::types::Frame& output
    ) override;

};


}

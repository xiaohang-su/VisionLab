#pragma once

#include "../core/core_types.h"


namespace visionlab::vision {


class VisionProcessor
{

public:

    virtual ~VisionProcessor() = default;


    virtual const char* name() const = 0;


    virtual bool process(
        const core::types::Frame& input,
        core::types::Frame& output
    ) = 0;

};


}

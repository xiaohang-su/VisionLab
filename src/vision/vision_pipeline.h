#pragma once

#include "vision_processor.h"

#include "../core/core_types.h"

#include <vector>


namespace visionlab::vision {


class VisionPipeline
{

public:

    VisionPipeline();

    ~VisionPipeline();


    void add_processor(
        VisionProcessor* processor
    );


    bool process(
        const core::types::Frame& input,
        core::types::Frame& output
    );


private:

    std::vector<VisionProcessor*> processors_;

    core::types::Frame buffer_a_;

    core::types::Frame buffer_b_;

};


}

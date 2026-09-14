#include "vision_pipeline.h"


namespace visionlab::vision {


VisionPipeline::VisionPipeline() = default;


VisionPipeline::~VisionPipeline() = default;



void VisionPipeline::add_processor(
    VisionProcessor* processor
)
{
    if (processor != nullptr)
    {
        processors_.push_back(processor);
    }
}



bool VisionPipeline::process(
    const core::types::Frame& input,
    core::types::Frame& output
)
{
    if (processors_.empty())
    {
        output = input;
        return true;
    }


    const std::size_t count = processors_.size();

    const core::types::Frame* src = &input;
    core::types::Frame* dst = nullptr;
    bool use_buffer_a = true;


    for (std::size_t i = 0; i < count; ++i)
    {
        if (i == count - 1)
        {
            dst = &output;
        }
        else if (use_buffer_a)
        {
            dst = &buffer_a_;
        }
        else
        {
            dst = &buffer_b_;
        }


        if (!processors_[i]->process(*src, *dst))
        {
            return false;
        }


        src = dst;
        use_buffer_a = !use_buffer_a;
    }


    return true;
}


}

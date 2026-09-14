#include "pass_through_processor.h"


namespace visionlab::vision {


const char* PassThroughProcessor::name() const
{
    return "PassThrough";
}



bool PassThroughProcessor::process(
    const core::types::Frame& input,
    core::types::Frame& output
)
{
    output = input;

    return true;
}


}

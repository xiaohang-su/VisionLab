#include "capture.h"


namespace visionlab::capture {


Capture::Capture()
    :
    running_(false),
    frame_count_(0)
{
}


Capture::~Capture()
{
    if (running_)
    {
        stop();
    }
}



const char* Capture::name() const
{
    return "Capture";
}



bool Capture::initialize()
{
    frame_count_ = 0;

    return true;
}



bool Capture::start()
{
    running_ = true;

    return true;
}



bool Capture::stop()
{
    running_ = false;

    return true;
}



bool Capture::get_frame(
    core::types::Frame& frame
)
{

    if (!running_)
    {
        return false;
    }


    frame_count_++;


    frame.id = core::types::Id(frame_count_);


    frame.width = 1920;

    frame.height = 1080;


    return true;
}



}
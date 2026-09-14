#include "capture.h"


namespace visionlab::capture {


Capture::Capture()
    :
    running_(false),
    frame_count_(0),
    source_(nullptr)
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

    if (source_ != nullptr)
    {
        return source_->open();
    }

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

    if (source_ != nullptr)
    {
        source_->close();
    }

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


    if (source_ != nullptr)
    {
        if (!source_->read(frame))
        {
            return false;
        }

        frame_count_++;

        frame.id = core::types::Id(frame_count_);

        return true;
    }


    // Mock behavior when no source is set
    frame_count_++;

    frame.id = core::types::Id(frame_count_);

    frame.width = 1920;

    frame.height = 1080;

    return true;
}



void Capture::set_source(
    CaptureSource* source
)
{
    source_ = source;
}



}

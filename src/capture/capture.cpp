#include "capture.h"


namespace visionlab::capture {


Capture::Capture()
    :
    running_(false)
{
}


Capture::~Capture()
{
    if(running_)
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



}
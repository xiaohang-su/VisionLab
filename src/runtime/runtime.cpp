#include "runtime.h"

#include <string>


namespace visionlab::runtime {


Runtime::Runtime(RuntimeContext* context)
    :
    state_(RuntimeState::Created),
    context_(context)
{
}


Runtime::~Runtime()
{
    if (state_ == RuntimeState::Running) {
        stop();
    }
}


const char* Runtime::name() const
{
    return "Runtime";
}


bool Runtime::initialize()
{
    if (context_ == nullptr) {
        return false;
    }


    if (state_ != RuntimeState::Created)
    {
        return false;
    }


    if (!context_->logger()->initialize())
    {
        return false;
    }


    state_ = RuntimeState::Initialized;


    context_->logger()->info(
        "Runtime initialized."
    );


    return true;
}



bool Runtime::start()
{
    if (state_ != RuntimeState::Initialized)
    {
        return false;
    }


    state_ = RuntimeState::Running;


    context_->logger()->info(
        "Runtime started."
    );


    return true;
}



bool Runtime::stop()
{
    if (state_ != RuntimeState::Running)
    {
        return false;
    }


    state_ = RuntimeState::Stopped;


    context_->logger()->info(
        "Runtime stopped."
    );


    return true;
}



RuntimeState Runtime::state() const
{
    return state_;
}



bool Runtime::is_running() const
{
    return state_ == RuntimeState::Running;
}


}
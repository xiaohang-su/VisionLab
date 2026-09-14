#include "runtime.h"

#include "../logging/logger.h"

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
        context_->logger()->warning(
            "Runtime initialize failed: current state is "
            + std::string(to_string(state_))
            + ", expected Created."
        );

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
        context_->logger()->warning(
            "Runtime start failed: current state is "
            + std::string(to_string(state_))
            + ", expected Initialized."
        );

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
        context_->logger()->warning(
            "Runtime stop failed: current state is "
            + std::string(to_string(state_))
            + ", expected Running."
        );

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
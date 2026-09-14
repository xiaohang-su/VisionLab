#include "runtime.h"

#include <string>

namespace visionlab::runtime {

Runtime::Runtime()
    : state_(RuntimeState::Created),
      logger_("logs/visionlab.log") {
}

Runtime::~Runtime() {
    if (state_ == RuntimeState::Running) {
        stop();
    }
}

const char* Runtime::name() const {
    return "Runtime";
}

bool Runtime::initialize() {
    if (state_ != RuntimeState::Created) {
        logger_.error(
            "Cannot initialize from state " +
            std::string(
                visionlab::core::types::to_string(state_)
            )
        );

        return false;
    }

    if (!logger_.initialize()) {
        return false;
    }

    state_ = RuntimeState::Initialized;

    logger_.info("Runtime initialized.");

    return true;
}

bool Runtime::start() {
    if (state_ != RuntimeState::Initialized) {
        logger_.error(
            "Cannot start from state " +
            std::string(
                visionlab::core::types::to_string(state_)
            )
        );

        return false;
    }

    state_ = RuntimeState::Running;

    logger_.info("Runtime started.");

    return true;
}

bool Runtime::stop() {
    if (state_ != RuntimeState::Running) {
        logger_.error(
            "Cannot stop from state " +
            std::string(
                visionlab::core::types::to_string(state_)
            )
        );

        return false;
    }

    state_ = RuntimeState::Stopped;

    logger_.info("Runtime stopped.");

    return true;
}

RuntimeState Runtime::state() const {
    return state_;
}

bool Runtime::is_running() const {
    return state_ == RuntimeState::Running;
}

}
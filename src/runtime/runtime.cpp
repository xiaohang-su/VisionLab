#include "runtime.h"
#include <iostream>

namespace visionlab::runtime {

Runtime::Runtime() : state_(RuntimeState::Created) {}

Runtime::~Runtime() {}

bool Runtime::initialize() {
    if (state_ != RuntimeState::Created) {
        std::cerr << "Error: Cannot initialize from state " 
                  << visionlab::core::types::to_string(state_) << std::endl;
        return false;
    }
    state_ = RuntimeState::Initialized;
    return true;
}

bool Runtime::start() {
    if (state_ != RuntimeState::Initialized) {
        std::cerr << "Error: Cannot start from state " 
                  << visionlab::core::types::to_string(state_) << std::endl;
        return false;
    }
    state_ = RuntimeState::Running;
    return true;
}

bool Runtime::stop() {
    if (state_ != RuntimeState::Running) {
        std::cerr << "Error: Cannot stop from state " 
                  << visionlab::core::types::to_string(state_) << std::endl;
        return false;
    }
    state_ = RuntimeState::Stopped;
    return true;
}

RuntimeState Runtime::state() const {
    return state_;
}

bool Runtime::is_running() const {
    return state_ == RuntimeState::Running;
}

}

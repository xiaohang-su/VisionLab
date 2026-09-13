#pragma once

#include "../core/core_types.h"

namespace visionlab::runtime {

using RuntimeState = visionlab::core::types::RuntimeState;

class Runtime {
public:
    Runtime();
    ~Runtime();

    bool initialize();
    bool start();
    bool stop();

    RuntimeState state() const;
    bool is_running() const;

private:
    RuntimeState state_;
};

}

#pragma once

#include "../core/core_types.h"
#include "../core/module.h"
#include "../logging/logger.h"

namespace visionlab::runtime {

using RuntimeState = visionlab::core::types::RuntimeState;

class Runtime : public visionlab::core::Module {
public:
    Runtime();
    ~Runtime();

    bool initialize() override;
    bool start() override;
    bool stop() override;

    RuntimeState state() const;
    bool is_running() const;

private:
    RuntimeState state_;
    visionlab::logging::Logger logger_;
};

}
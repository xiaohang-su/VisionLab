#pragma once

#include "../core/core_types.h"
#include "../core/module.h"

#include "runtime_context.h"


namespace visionlab::runtime {


using RuntimeState = visionlab::core::types::RuntimeState;


class Runtime : public visionlab::core::Module {

public:

    Runtime(RuntimeContext* context);

    ~Runtime();


    const char* name() const override;


    bool initialize() override;

    bool start() override;

    bool stop() override;


    RuntimeState state() const;

    bool is_running() const;


private:

    RuntimeState state_;

    RuntimeContext* context_;

};


}
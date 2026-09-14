#pragma once

#include "../core/module.h"
#include "../core/core_types.h"


namespace visionlab::capture {


class Capture : public core::Module
{

public:

    Capture();
    ~Capture() override;


    const char* name() const override;


    bool initialize() override;

    bool start() override;

    bool stop() override;


private:

    bool running_;

};


}
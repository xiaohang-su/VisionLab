#pragma once

namespace visionlab::core {

class Module {
public:
    virtual ~Module() = default;

    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
};

}

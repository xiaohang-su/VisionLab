#pragma once

#include "core/module_manager.h"
#include "runtime/runtime.h"
#include "logging/logger.h"
#include "config/config.h"

namespace visionlab {

class Application
{
public:

    Application();

    bool initialize();

    void run();

    void shutdown();

private:

    config::Config config_;
    logging::Logger logger_;
    runtime::Runtime runtime_;
    core::ModuleManager module_manager_;

    bool initialized_;
};

}

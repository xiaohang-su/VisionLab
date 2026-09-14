#pragma once

#include "module_manager.h"

#include "runtime/runtime.h"
#include "runtime/runtime_context.h"

#include "../capture/capture.h"

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

    runtime::RuntimeContext runtime_context_;

    runtime::Runtime runtime_;

    capture::Capture capture_;

    core::ModuleManager module_manager_;


    bool initialized_;

};


}
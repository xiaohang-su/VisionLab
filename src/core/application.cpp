#include "application.h"

namespace visionlab {

Application::Application()
    :
    logger_("logs/visionlab.log"),
    initialized_(false)
{
}

bool Application::initialize()
{
    if (!logger_.initialize())
    {
        return false;
    }

    logger_.info("VisionLab starting...");

    module_manager_.add(&runtime_);

    if (!module_manager_.initialize_all())
    {
        logger_.error("Module initialization failed.");
        return false;
    }

    initialized_ = true;

    return true;
}

void Application::run()
{
    if (!initialized_)
    {
        return;
    }

    if (!module_manager_.start_all())
    {
        logger_.error("Module start failed.");
        return;
    }
}

void Application::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    module_manager_.stop_all();

    logger_.info("VisionLab shutdown");

    initialized_ = false;
}

}

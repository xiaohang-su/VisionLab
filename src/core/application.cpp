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

    initialized_ = true;

    return true;
}

void Application::run()
{
    if (!initialized_)
    {
        return;
    }

    logger_.info("Runtime started");

    runtime_.initialize();

    runtime_.run();
}

void Application::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    runtime_.shutdown();

    logger_.info("VisionLab shutdown");
}

}

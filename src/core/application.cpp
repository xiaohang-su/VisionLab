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

    if (!runtime_.initialize())
    {
        logger_.error("Runtime initialization failed.");
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

    runtime_.start();
}

void Application::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    runtime_.stop();

    logger_.info("VisionLab shutdown");

    initialized_ = false;
}

}


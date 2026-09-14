#include "application.h"


namespace visionlab {


Application::Application()
    :
    logger_("logs/visionlab.log"),
    runtime_context_(&logger_, &config_),
    runtime_(&runtime_context_),
    initialized_(false)
{
    module_manager_.add(&runtime_);
}


bool Application::initialize()
{
    if (!logger_.initialize())
    {
        return false;
    }


    logger_.info("VisionLab starting...");


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
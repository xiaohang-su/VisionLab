#include "application.h"


namespace visionlab {


Application::Application()
    :
    logger_("logs/visionlab.log"),
    runtime_context_(&logger_, &config_),
    runtime_(&runtime_context_),
    capture_(),
    module_manager_(&logger_),
    initialized_(false)
{

    module_manager_.add(&runtime_);

    module_manager_.add(&capture_);

}



bool Application::initialize()
{

    if (!logger_.initialize())
    {
        return false;
    }


    logger_.info("VisionLab starting...");


    if (config_.load("config/default.ini"))
    {
        logger_.info(
            "Configuration loaded: "
            + std::to_string(config_.size())
            + " entries."
        );
    }
    else
    {
        logger_.info("No configuration file found, using defaults.");
    }


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
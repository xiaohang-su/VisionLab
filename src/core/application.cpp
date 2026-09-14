#include "application.h"

#include <chrono>
#include <thread>


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


    logger_.info("Frame loop started.");


    running_ = true;


    std::uint64_t frame_count = 0;

    const auto start_time = std::chrono::steady_clock::now();


    while (running_)
    {

        if (!capture_.get_frame(original_frame_))
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );

            continue;
        }


        if (!vision_pipeline_.process(
                original_frame_,
                processed_frame_
            ))
        {
            logger_.warning(
                "Vision pipeline processing failed."
            );

            continue;
        }


        if (detector_ != nullptr)
        {
            if (!detector_->detect(
                    processed_frame_,
                    detection_result_
                ))
            {
                logger_.warning(
                    "Detection failed."
                );
            }
        }


        frame_count++;


        if (frame_count % 100 == 0)
        {
            logger_.info(
                "Frames processed: "
                + std::to_string(frame_count)
            );
        }

    }


    running_ = false;


    const auto end_time = std::chrono::steady_clock::now();

    const auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time
        );

    const double fps =
        (duration.count() > 0)
            ? (static_cast<double>(frame_count) * 1000.0
               / static_cast<double>(duration.count()))
            : 0.0;


    logger_.info(
        "Frame loop stopped. Total frames: "
        + std::to_string(frame_count)
        + ", "
        + std::to_string(fps)
        + " FPS"
    );

}



void Application::shutdown()
{

    if (!initialized_)
    {
        return;
    }


    running_ = false;


    module_manager_.stop_all();


    logger_.info("VisionLab shutdown");


    initialized_ = false;

}



void Application::set_capture_source(
    capture::CaptureSource* source
)
{
    capture_.set_source(source);
}



vision::VisionPipeline& Application::vision_pipeline()
{
    return vision_pipeline_;
}



void Application::set_detector(
    detection::Detector* detector
)
{
    detector_ = detector;
}



void Application::request_stop()
{
    running_ = false;
}



}
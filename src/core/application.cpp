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

        // Frame lifetime: capture allocation tracking
        const std::size_t capacity_before =
            original_frame_.data.capacity();


        core::Timer capture_timer;

        if (!capture_.get_frame(original_frame_))
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );

            continue;
        }

        metrics_.capture_ms = capture_timer.elapsed_ms();


        // Frame lifetime: detect vector reallocation
        if (original_frame_.data.capacity() > capacity_before)
        {
            metrics_.frame_allocation_count++;
        }


        core::Timer vision_timer;

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

        metrics_.vision_ms = vision_timer.elapsed_ms();


        // Frame lifetime: count data copy from input to output
        metrics_.frame_copy_bytes +=
            static_cast<std::uint64_t>(processed_frame_.data.size());
        metrics_.frame_data_bytes =
            static_cast<std::uint64_t>(processed_frame_.data.size());


        core::Timer detection_timer;

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

        metrics_.detection_ms = detection_timer.elapsed_ms();


        core::Timer tracking_timer;

        if (tracker_ != nullptr)
        {
            if (!tracker_->update(
                    detection_result_,
                    track_result_
                ))
            {
                logger_.warning(
                    "Tracking update failed."
                );
            }
        }

        metrics_.tracking_ms = tracking_timer.elapsed_ms();


        core::Timer analysis_timer;

        if (analyzer_ != nullptr)
        {
            if (!analyzer_->analyze(
                    detection_result_,
                    track_result_,
                    analysis_result_
                ))
            {
                logger_.warning(
                    "Analysis failed."
                );
            }
        }

        metrics_.analysis_ms = analysis_timer.elapsed_ms();


        // Update UI context with read-only references
        ui_context_.frame = &processed_frame_;
        ui_context_.detections = &detection_result_;
        ui_context_.tracks = &track_result_;
        ui_context_.analysis = &analysis_result_;
        ui_context_.frame_count = frame_count;
        ui_context_.metrics = &metrics_;

        metrics_.frame_count = frame_count;
        metrics_.logger_ms = logger_.total_log_ms();
        metrics_.log_count = logger_.log_count();

        {
            const auto now = std::chrono::steady_clock::now();
            const auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - start_time
                );
            metrics_.fps =
                (elapsed.count() > 0)
                    ? (static_cast<double>(frame_count) * 1000.0
                       / static_cast<double>(elapsed.count()))
                    : 0.0;
            ui_context_.fps = static_cast<float>(metrics_.fps);
        }


        // Frame lifetime: count texture upload bytes (BGRA8 = 4 bytes/pixel)
        if (processed_frame_.width > 0 && processed_frame_.height > 0)
        {
            metrics_.texture_upload_bytes +=
                static_cast<std::uint64_t>(processed_frame_.width) *
                static_cast<std::uint64_t>(processed_frame_.height) * 4;
            metrics_.texture_upload_count++;
        }


        core::Timer render_timer;

        if (!ui_.render(ui_context_))
        {
            // Renderer requested close (e.g. window closed)
            request_stop();
        }

        metrics_.render_ms = render_timer.elapsed_ms();


        // V0.9.3: read texture upload time from UI feedback
        metrics_.texture_upload_ms = ui_context_.texture_upload_ms;


        // V0.9.3: estimate vision copy bandwidth (MB/s)
        if (metrics_.vision_ms > 0.0
            && processed_frame_.data.size() > 0)
        {
            const double bytes_per_frame =
                static_cast<double>(processed_frame_.data.size());
            const double seconds = metrics_.vision_ms / 1000.0;
            metrics_.vision_copy_bandwidth_MBps =
                (bytes_per_frame / (1024.0 * 1024.0)) / seconds;
        }
        else
        {
            metrics_.vision_copy_bandwidth_MBps = 0.0;
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



void Application::set_tracker(
    tracking::Tracker* tracker
)
{
    tracker_ = tracker;
}



void Application::set_analyzer(
    analysis::Analyzer* analyzer
)
{
    analyzer_ = analyzer;
}



void Application::set_renderer(
    ui::UIRenderer* renderer
)
{
    ui_.set_renderer(renderer);
}



void Application::request_stop()
{
    running_ = false;
}



}
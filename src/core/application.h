#pragma once

#include "module_manager.h"

#include "runtime/runtime.h"
#include "runtime/runtime_context.h"

#include "../capture/capture.h"

#include "../vision/vision_pipeline.h"

#include "../detection/detector.h"

#include "../tracking/tracker.h"

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


    void request_stop();


    void set_capture_source(
        capture::CaptureSource* source
    );


    vision::VisionPipeline& vision_pipeline();


    void set_detector(
        detection::Detector* detector
    );


    void set_tracker(
        tracking::Tracker* tracker
    );


private:

    config::Config config_;

    logging::Logger logger_;

    runtime::RuntimeContext runtime_context_;

    runtime::Runtime runtime_;

    capture::Capture capture_;

    vision::VisionPipeline vision_pipeline_;

    core::ModuleManager module_manager_;


    bool initialized_;

    bool running_ = false;

    core::types::Frame original_frame_;

    core::types::Frame processed_frame_;

    detection::Detector* detector_ = nullptr;

    detection::DetectionResult detection_result_;

    tracking::Tracker* tracker_ = nullptr;

    tracking::TrackResult track_result_;

};


}
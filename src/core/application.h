#pragma once

#include "module_manager.h"

#include "runtime/runtime.h"
#include "runtime/runtime_context.h"
#include "runtime/runtime_metrics.h"

#include "timer.h"

#include "../capture/capture.h"

#include "../vision/vision_pipeline.h"

#include "../detection/detector.h"

#include "../tracking/tracker.h"

#include "../analysis/analyzer.h"

#include "../ui/ui.h"
#include "../ui/ui_context.h"

#include "../auth/auth_service.h"
#include "../platform/platform_service.h"
#include "../inference/inference_backend.h"

#include "logging/logger.h"
#include "config/config.h"

#include <string>
#include <vector>


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


    void set_analyzer(
        analysis::Analyzer* analyzer
    );


    void set_renderer(
        ui::UIRenderer* renderer
    );


    // V1.1 Platform services
    void set_auth_service(auth::AuthService* service);
    void set_platform_service(platform::PlatformService* service);
    void set_inference_backend(inference::InferenceBackend* backend);


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

    analysis::Analyzer* analyzer_ = nullptr;

    analysis::AnalysisResult analysis_result_;

    ui::UI ui_;

    ui::UIContext ui_context_;

    runtime::RuntimeMetrics metrics_;

    // V1.1 Platform services (injected, non-owning)
    auth::AuthService* auth_service_ = nullptr;
    platform::PlatformService* platform_service_ = nullptr;
    inference::InferenceBackend* inference_backend_ = nullptr;

    // Cached platform data (updated each frame)
    platform::WeatherInfo weather_cache_;
    platform::PlatformInfo platform_info_cache_;
    std::vector<platform::Announcement> announcements_cache_;
    std::string time_string_cache_;

};


}
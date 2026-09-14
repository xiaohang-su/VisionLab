#include "core/application.h"

#include "vision/pass_through_processor.h"
#include "detection/mock_detector.h"
#include "tracking/mock_tracker.h"
#include "analysis/mock_analyzer.h"
#include "ui/mock_ui.h"

#ifdef _WIN32
#include "capture/screen_capture_source.h"
#else
#include "capture/mock_capture_source.h"
#endif


int main()
{
    visionlab::Application app;

#ifdef _WIN32
    visionlab::capture::ScreenCaptureSource source;
#else
    visionlab::capture::MockCaptureSource source;
#endif

    app.set_capture_source(&source);


    visionlab::vision::PassThroughProcessor passthrough;

    app.vision_pipeline().add_processor(&passthrough);


    visionlab::detection::MockDetector detector;

    app.set_detector(&detector);


    visionlab::tracking::MockTracker tracker;

    app.set_tracker(&tracker);


    visionlab::analysis::MockAnalyzer analyzer;

    app.set_analyzer(&analyzer);


    visionlab::ui::MockUI mock_ui;

    app.set_renderer(&mock_ui);


    if (!app.initialize())
    {
        return -1;
    }

    app.run();

    app.shutdown();

    return 0;
}

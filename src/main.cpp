#include "core/application.h"

#include "vision/pass_through_processor.h"
#include "detection/mock_detector.h"
#include "tracking/mock_tracker.h"
#include "analysis/mock_analyzer.h"

#ifdef VISIONLAB_HAS_IMGUI
#include "ui/backends/imgui_renderer.h"
#else
#include "ui/mock_ui.h"
#endif

// V0.9.6: use explicit feature flag instead of broad _WIN32 check
#ifdef VISIONLAB_ENABLE_SCREEN_CAPTURE
#include "capture/screen_capture_source.h"
#else
#include "capture/mock_capture_source.h"
#endif


int main()
{
    visionlab::Application app;

#ifdef VISIONLAB_ENABLE_SCREEN_CAPTURE
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


#ifdef VISIONLAB_HAS_IMGUI
    visionlab::ui::ImGuiRenderer renderer;

    if (!renderer.initialize(1280, 720))
    {
        return -1;
    }

    app.set_renderer(&renderer);
#else
    visionlab::ui::MockUI mock_ui;

    app.set_renderer(&mock_ui);
#endif


    if (!app.initialize())
    {
        return -1;
    }

    app.run();

    app.shutdown();

#ifdef VISIONLAB_HAS_IMGUI
    renderer.shutdown();
#endif

    return 0;
}

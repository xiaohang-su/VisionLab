#include "core/application.h"

#include "vision/pass_through_processor.h"
#include "detection/inference_detector.h"
#include "tracking/mock_tracker.h"
#include "analysis/mock_analyzer.h"

#include "inference/mock_inference_backend.h"
#include "auth/mock_auth_service.h"
#include "platform/mock_platform_service.h"

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

    // === Capture ===
#ifdef VISIONLAB_ENABLE_SCREEN_CAPTURE
    visionlab::capture::ScreenCaptureSource source;
#else
    visionlab::capture::MockCaptureSource source;
#endif
    app.set_capture_source(&source);

    // === Vision Pipeline ===
    visionlab::vision::PassThroughProcessor passthrough;
    app.vision_pipeline().add_processor(&passthrough);

    // === Inference Backend (V1.1) ===
    visionlab::inference::MockInferenceBackend inference_backend;
    inference_backend.load("models/mock_model");

    // === Detection (delegates to inference backend) ===
    visionlab::detection::InferenceDetector detector(&inference_backend);
    app.set_detector(&detector);
    app.set_inference_backend(&inference_backend);

    // === Tracking ===
    visionlab::tracking::MockTracker tracker;
    app.set_tracker(&tracker);

    // === Analysis ===
    visionlab::analysis::MockAnalyzer analyzer;
    app.set_analyzer(&analyzer);

    // === Auth Service (V1.1) ===
    visionlab::auth::MockAuthService auth_service;
    app.set_auth_service(&auth_service);

    // === Platform Service (V1.1) ===
    visionlab::platform::MockPlatformService platform_service;
    app.set_platform_service(&platform_service);

    // === UI Renderer ===
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

    // === Auto-login for Mock mode (V1.1) ===
    // In production, user enters card key in UI login page.
    // Mock mode auto-logs-in so the platform UI is visible immediately.
    auth_service.verify_card("MOCK-AUTO-LOGIN");

    // === Run ===
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

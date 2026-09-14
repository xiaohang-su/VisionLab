#include "core/application.h"

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

    if (!app.initialize())
    {
        return -1;
    }

    app.run();

    app.shutdown();

    return 0;
}

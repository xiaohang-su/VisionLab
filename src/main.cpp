#include "core/application.h"

int main()
{
    visionlab::Application app;

    if (!app.initialize())
    {
        return -1;
    }

    app.run();

    app.shutdown();

    return 0;
}

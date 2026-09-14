#include "mock_ui.h"

#include <cstdio>


namespace visionlab::ui {


const char* MockUI::name() const
{
    return "MockUI";
}



bool MockUI::render(
    const UIContext& context
)
{
    // MockUI: no real window or rendering.
    // Outputs frame statistics to validate data flow.

    if (context.frame_count % 100 != 0)
    {
        return true;
    }


    int detection_count = 0;

    if (context.detections != nullptr)
    {
        detection_count =
            static_cast<int>(context.detections->detections.size());
    }


    int track_count = 0;

    if (context.tracks != nullptr)
    {
        track_count =
            static_cast<int>(context.tracks->tracks.size());
    }


    int active_track_count = 0;

    if (context.analysis != nullptr)
    {
        active_track_count = context.analysis->active_track_count;
    }


    std::printf(
        "[MockUI] frame=%llu fps=%.1f detections=%d tracks=%d active_tracks=%d\n",
        static_cast<unsigned long long>(context.frame_count),
        context.fps,
        detection_count,
        track_count,
        active_track_count
    );


    return true;
}


}

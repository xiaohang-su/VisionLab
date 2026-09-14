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
        "[MockUI] frame=%llu fps=%.1f detections=%d tracks=%d active_tracks=%d",
        static_cast<unsigned long long>(context.frame_count),
        context.fps,
        detection_count,
        track_count,
        active_track_count
    );


    if (context.metrics != nullptr)
    {
        std::printf(
            " | cap=%.2fms vis=%.2fms det=%.2fms trk=%.2fms ana=%.2fms rnd=%.2fms log=%.2fms(%llu)",
            context.metrics->capture_ms,
            context.metrics->vision_ms,
            context.metrics->detection_ms,
            context.metrics->tracking_ms,
            context.metrics->analysis_ms,
            context.metrics->render_ms,
            context.metrics->logger_ms,
            static_cast<unsigned long long>(context.metrics->log_count)
        );

        std::printf(
            " | frame_data=%.2fMB alloc=%llu copy=%.2fMB upload=%.2fMB(%llu)",
            context.metrics->frame_data_bytes / (1024.0 * 1024.0),
            static_cast<unsigned long long>(context.metrics->frame_allocation_count),
            context.metrics->frame_copy_bytes / (1024.0 * 1024.0),
            context.metrics->texture_upload_bytes / (1024.0 * 1024.0),
            static_cast<unsigned long long>(context.metrics->texture_upload_count)
        );

        std::printf(
            " | upload=%.3fms vis_bw=%.1fMB/s",
            context.metrics->texture_upload_ms,
            context.metrics->vision_copy_bandwidth_MBps
        );
    }


    std::printf("\n");


    return true;
}


}

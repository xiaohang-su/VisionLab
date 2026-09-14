#include "mock_ui.h"

#include <algorithm>
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


        // V0.9.4: Bottleneck ranking (UI-local, no core changes)
        struct StageCost {
            const char* name;
            double ms;
        };

        StageCost stages[6] = {
            {"Capture",   context.metrics->capture_ms},
            {"Vision",    context.metrics->vision_ms},
            {"Detection", context.metrics->detection_ms},
            {"Tracking",  context.metrics->tracking_ms},
            {"Analysis",  context.metrics->analysis_ms},
            {"Render",    context.metrics->render_ms},
        };

        const double frame_time =
            context.metrics->capture_ms +
            context.metrics->vision_ms +
            context.metrics->detection_ms +
            context.metrics->tracking_ms +
            context.metrics->analysis_ms +
            context.metrics->render_ms;

        std::sort(stages, stages + 6,
            [](const StageCost& a, const StageCost& b) {
                return a.ms > b.ms;
            });

        std::printf("\n[Bottleneck] ");
        for (int i = 0; i < 3; i++)
        {
            const double ratio =
                (frame_time > 0.0) ? (stages[i].ms / frame_time * 100.0) : 0.0;
            std::printf("%d.%s %.1f%%  ",
                i + 1, stages[i].name, ratio);
        }
    }


    std::printf("\n");


    return true;
}


}

#include "mock_analyzer.h"

#include <map>


namespace visionlab::analysis {


const char* MockAnalyzer::name() const
{
    return "MockAnalyzer";
}



bool MockAnalyzer::analyze(
    const detection::DetectionResult& detections,
    const tracking::TrackResult& tracks,
    AnalysisResult& result
)
{
    // Inherit frame_id from DetectionResult
    result.frame_id = detections.frame_id;


    // Count detections
    result.detection_count =
        static_cast<int>(detections.detections.size());


    // Count active tracks
    int active_count = 0;

    for (const auto& track : tracks.tracks)
    {
        if (track.active)
        {
            active_count++;
        }
    }

    result.active_track_count = active_count;


    // Simple class statistics
    result.class_stats.clear();

    std::map<int, int> class_count;

    for (const auto& detection : detections.detections)
    {
        class_count[detection.class_id]++;
    }

    for (const auto& [class_id, count] : class_count)
    {
        ClassStat stat;

        stat.class_id = class_id;

        stat.count = count;

        result.class_stats.push_back(stat);
    }


    // Processing time not measured in V0.7
    result.processing_time_ms = 0.0;


    return true;
}


}

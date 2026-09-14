#include "mock_tracker.h"


namespace visionlab::tracking {


const char* MockTracker::name() const
{
    return "MockTracker";
}



bool MockTracker::update(
    const detection::DetectionResult& detections,
    TrackResult& result
)
{
    // Inherit frame_id from DetectionResult
    result.frame_id = detections.frame_id;

    result.tracks.clear();


    for (const auto& detection : detections.detections)
    {
        Track track;

        track.id = next_track_id_++;

        track.bbox = detection.bbox;

        track.class_id = detection.class_id;

        track.confidence = detection.confidence;

        track.active = true;


        result.tracks.push_back(track);
    }


    return true;
}


}

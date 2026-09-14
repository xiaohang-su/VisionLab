#pragma once

#include "tracker.h"


namespace visionlab::tracking {


class MockTracker
    : public Tracker
{

public:

    const char* name() const override;


    bool update(
        const detection::DetectionResult& detections,
        TrackResult& result
    ) override;


private:

    int next_track_id_ = 0;

};


}

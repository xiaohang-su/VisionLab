#pragma once

#include "track_result.h"

#include "../detection/detection_result.h"


namespace visionlab::tracking {


class Tracker
{

public:

    virtual ~Tracker() = default;


    virtual const char* name() const = 0;


    virtual bool update(
        const detection::DetectionResult& detections,
        TrackResult& result
    ) = 0;

};


}

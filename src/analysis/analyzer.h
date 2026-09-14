#pragma once

#include "analysis_result.h"

#include "../detection/detection_result.h"
#include "../tracking/track_result.h"


namespace visionlab::analysis {


class Analyzer
{

public:

    virtual ~Analyzer() = default;


    virtual const char* name() const = 0;


    virtual bool analyze(
        const detection::DetectionResult& detections,
        const tracking::TrackResult& tracks,
        AnalysisResult& result
    ) = 0;

};


}

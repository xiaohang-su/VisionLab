#pragma once

#include "analyzer.h"


namespace visionlab::analysis {


class MockAnalyzer
    : public Analyzer
{

public:

    const char* name() const override;


    bool analyze(
        const detection::DetectionResult& detections,
        const tracking::TrackResult& tracks,
        AnalysisResult& result
    ) override;

};


}

#include "mock_detector.h"


namespace visionlab::detection {


const char* MockDetector::name() const
{
    return "MockDetector";
}



bool MockDetector::detect(
    const core::types::Frame& frame,
    DetectionResult& result
)
{
    result.frame_id = frame.id;

    result.detections.clear();


    // Fixed detection: centered bounding box covering 1/4 of frame
    const float box_width =
        static_cast<float>(frame.width) * 0.25f;

    const float box_height =
        static_cast<float>(frame.height) * 0.25f;

    Detection detection;

    detection.bbox.x =
        (static_cast<float>(frame.width) - box_width) * 0.5f;

    detection.bbox.y =
        (static_cast<float>(frame.height) - box_height) * 0.5f;

    detection.bbox.width = box_width;

    detection.bbox.height = box_height;

    detection.class_id = 0;

    detection.confidence = 0.9f;


    result.detections.push_back(detection);


    return true;
}


}

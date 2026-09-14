#pragma once

#include "detector.h"
#include "../inference/inference_backend.h"


namespace visionlab::detection {


// Detector that delegates to an InferenceBackend.
// This bridges the frozen Detector interface with the new inference layer.
// MockDetector remains available for simple testing without a backend.
class InferenceDetector : public Detector
{
public:

    InferenceDetector() = default;

    explicit InferenceDetector(inference::InferenceBackend* backend)
        : backend_(backend) {}

    void set_backend(inference::InferenceBackend* backend)
    {
        backend_ = backend;
    }

    inference::InferenceBackend* backend() const { return backend_; }

    const char* name() const override
    {
        return backend_ ? backend_->name() : "InferenceDetector(no backend)";
    }

    bool detect(
        const core::types::Frame& frame,
        DetectionResult& result
    ) override
    {
        if (!backend_)
        {
            result.frame_id = frame.id;
            result.detections.clear();
            return false;
        }

        if (!backend_->is_loaded())
        {
            result.frame_id = frame.id;
            result.detections.clear();
            return false;
        }

        return backend_->infer(frame, result);
    }

private:

    inference::InferenceBackend* backend_ = nullptr;
};


}

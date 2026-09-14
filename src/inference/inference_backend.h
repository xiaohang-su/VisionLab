#pragma once

#include "model_info.h"

#include "../core/core_types.h"
#include "../detection/detection_result.h"

#include <string>


namespace visionlab::inference {


// Abstract inference backend.
// Future implementations: OnnxBackend, TensorRTBackend, DirectMLBackend.
// Does NOT inherit Module — keeps inference independent of Runtime lifecycle.
class InferenceBackend
{
public:

    virtual ~InferenceBackend() = default;

    virtual const char* name() const = 0;

    virtual BackendType type() const = 0;

    // Load a model from file. Returns true on success.
    virtual bool load(const std::string& model_path) = 0;

    // Run inference on a frame, write detections to result.
    virtual bool infer(
        const core::types::Frame& input,
        detection::DetectionResult& result
    ) = 0;

    virtual void unload() = 0;

    virtual bool is_loaded() const = 0;

    virtual const ModelInfo& model_info() const = 0;

    virtual const InferenceMetrics& metrics() const = 0;
};


}

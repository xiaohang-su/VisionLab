#pragma once

#include "inference_backend.h"


namespace visionlab::inference {


// Mock inference backend. Produces a fixed center detection for testing.
// Replaces the old MockDetector logic; Detector now delegates to this.
class MockInferenceBackend : public InferenceBackend
{
public:

    const char* name() const override { return "MockInferenceBackend"; }

    BackendType type() const override { return BackendType::Mock; }

    bool load(const std::string& model_path) override
    {
        model_info_.name = model_path.empty() ? "mock_model" : model_path;
        model_info_.path = model_path;
        model_info_.format = ModelFormat::Unknown;
        model_info_.backend = BackendType::Mock;
        model_info_.input_width = 640;
        model_info_.input_height = 640;
        model_info_.num_classes = 80;
        model_info_.loaded = true;
        return true;
    }

    bool infer(
        const core::types::Frame& input,
        detection::DetectionResult& result
    ) override
    {
        if (!model_info_.loaded) return false;

        result.frame_id = input.id;
        result.detections.clear();

        // Produce a fixed center detection (same as old MockDetector)
        detection::Detection d;
        d.class_id = 0;
        d.confidence = 0.9f;
        d.bbox.x = static_cast<float>(input.width) * 0.35f;
        d.bbox.y = static_cast<float>(input.height) * 0.30f;
        d.bbox.width = static_cast<float>(input.width) * 0.30f;
        d.bbox.height = static_cast<float>(input.height) * 0.40f;
        result.detections.push_back(d);

        metrics_.inference_count++;
        return true;
    }

    void unload() override
    {
        model_info_.loaded = false;
    }

    bool is_loaded() const override { return model_info_.loaded; }

    const ModelInfo& model_info() const override { return model_info_; }

    const InferenceMetrics& metrics() const override { return metrics_; }

private:

    ModelInfo model_info_;
    InferenceMetrics metrics_;
};


}

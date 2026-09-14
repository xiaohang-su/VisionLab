#pragma once

#include <string>
#include <cstdint>
#include <vector>


namespace visionlab::inference {


enum class BackendType
{
    Mock,
    OnnxRuntimeCPU,
    OnnxRuntimeCUDA,
    TensorRT,
    DirectML,
};


enum class ModelFormat
{
    Unknown,
    Onnx,
    Engine,   // TensorRT
    Pt,       // PyTorch (future)
};


struct ModelInfo
{
    std::string name;
    std::string path;
    ModelFormat format = ModelFormat::Unknown;
    BackendType backend = BackendType::Mock;

    int input_width = 640;
    int input_height = 640;
    int num_classes = 80;

    float confidence_threshold = 0.25f;
    float iou_threshold = 0.45f;

    bool loaded = false;
    std::string error_message;
};


struct InferenceMetrics
{
    double preprocess_ms = 0.0;
    double inference_ms = 0.0;
    double postprocess_ms = 0.0;
    uint64_t inference_count = 0;
};


}

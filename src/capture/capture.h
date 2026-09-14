#pragma once

#include "../core/module.h"
#include "../core/core_types.h"

#include "frame_provider.h"
#include "capture_source.h"

#include <cstdint>


namespace visionlab::capture {


class Capture 
    : public core::Module,
      public FrameProvider
{

public:

    Capture();

    ~Capture() override;


    const char* name() const override;


    bool initialize() override;

    bool start() override;

    bool stop() override;


    bool get_frame(
        core::types::Frame& frame
    ) override;


    void set_source(
        CaptureSource* source
    );


private:

    bool running_;

    std::uint64_t frame_count_;

    CaptureSource* source_;

};


}
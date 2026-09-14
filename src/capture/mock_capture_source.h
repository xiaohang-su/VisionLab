#pragma once

#include "capture_source.h"

#include <cstdint>


namespace visionlab::capture {


class MockCaptureSource
    : public CaptureSource
{

public:

    MockCaptureSource();

    ~MockCaptureSource() override;


    bool open() override;

    bool close() override;

    bool read(
        core::types::Frame& frame
    ) override;


private:

    bool opened_;

    std::uint64_t frame_count_;

};


}

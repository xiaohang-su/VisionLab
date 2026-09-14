#pragma once

#include "capture_source.h"


namespace visionlab::capture {


class ScreenCaptureSource
    : public CaptureSource
{

public:

    ScreenCaptureSource();

    ~ScreenCaptureSource() override;


    bool open() override;

    bool close() override;

    bool read(
        core::types::Frame& frame
    ) override;


private:

    void* impl_ = nullptr;

};


}

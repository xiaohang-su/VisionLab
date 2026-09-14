#pragma once

#include "../core/core_types.h"


namespace visionlab::capture {


class CaptureSource
{

public:

    virtual ~CaptureSource() = default;


    virtual bool open() = 0;


    virtual bool close() = 0;


    virtual bool read(
        core::types::Frame& frame
    ) = 0;

};


}
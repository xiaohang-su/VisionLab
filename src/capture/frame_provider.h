#pragma once

#include "../core/core_types.h"


namespace visionlab::capture {


class FrameProvider
{

public:

    virtual ~FrameProvider() = default;


    virtual bool get_frame(
        core::types::Frame& frame
    ) = 0;

};


}
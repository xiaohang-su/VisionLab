#pragma once

#include "../core/core_types.h"

#include <cstdint>


namespace visionlab::vision {


class ImageView
{

public:

    ImageView() = default;


    explicit ImageView(
        const core::types::Frame& frame
    );


    const std::uint8_t* data() const;


    std::uint32_t width() const;


    std::uint32_t height() const;


    core::types::PixelFormat format() const;


private:

    const core::types::Frame* frame_ = nullptr;

};


}

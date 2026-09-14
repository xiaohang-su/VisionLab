#include "mock_capture_source.h"


namespace visionlab::capture {


MockCaptureSource::MockCaptureSource()
    :
    opened_(false),
    frame_count_(0)
{
}


MockCaptureSource::~MockCaptureSource()
{
    close();
}



bool MockCaptureSource::open()
{
    opened_ = true;

    frame_count_ = 0;

    return true;
}



bool MockCaptureSource::close()
{
    opened_ = false;

    return true;
}



bool MockCaptureSource::read(
    core::types::Frame& frame
)
{
    if (!opened_)
    {
        return false;
    }


    frame_count_++;


    frame.width = 640;

    frame.height = 480;

    frame.format = core::types::PixelFormat::BGRA8;


    const std::size_t pixel_count =
        static_cast<std::size_t>(frame.width) *
        static_cast<std::size_t>(frame.height);

    const std::size_t byte_count = pixel_count * 4; // BGRA8 = 4 bytes per pixel

    frame.data.assign(byte_count, 0);


    // Fill with a simple gradient pattern for testing
    for (std::uint32_t y = 0; y < frame.height; ++y)
    {
        for (std::uint32_t x = 0; x < frame.width; ++x)
        {
            const std::size_t index =
                (static_cast<std::size_t>(y) * frame.width + x) * 4;

            frame.data[index + 0] = static_cast<std::uint8_t>(x % 256); // B
            frame.data[index + 1] = static_cast<std::uint8_t>(y % 256); // G
            frame.data[index + 2] = static_cast<std::uint8_t>(
                (x + y) % 256
            ); // R
            frame.data[index + 3] = 255; // A
        }
    }


    return true;
}



}

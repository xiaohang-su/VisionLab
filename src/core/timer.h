#pragma once

#include <chrono>


namespace visionlab::core {


class Timer
{

public:

    Timer()
    {
        start();
    }


    void start()
    {
        start_ = std::chrono::steady_clock::now();
    }


    double elapsed_ms() const
    {
        const auto now = std::chrono::steady_clock::now();

        const auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                now - start_
            );

        return static_cast<double>(duration.count()) / 1000.0;
    }


private:

    std::chrono::steady_clock::time_point start_;

};


}

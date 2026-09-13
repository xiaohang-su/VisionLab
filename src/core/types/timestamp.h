#pragma once

#include <chrono>
#include <cstdint>

namespace visionlab::core::types {

class Timestamp {
public:
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    using TimePoint = Clock::time_point;

    static Timestamp now() {
        return Timestamp(Clock::now());
    }

    explicit Timestamp(TimePoint tp = TimePoint{}) : time_point_(tp) {}

    TimePoint time_point() const { return time_point_; }

    std::int64_t milliseconds_since_epoch() const {
        auto duration = time_point_.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    std::int64_t microseconds_since_epoch() const {
        auto duration = time_point_.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    bool operator==(const Timestamp& other) const { return time_point_ == other.time_point_; }
    bool operator!=(const Timestamp& other) const { return time_point_ != other.time_point_; }
    bool operator<(const Timestamp& other) const { return time_point_ < other.time_point_; }
    bool operator<=(const Timestamp& other) const { return time_point_ <= other.time_point_; }
    bool operator>(const Timestamp& other) const { return time_point_ > other.time_point_; }
    bool operator>=(const Timestamp& other) const { return time_point_ >= other.time_point_; }

    Duration operator-(const Timestamp& other) const {
        return time_point_ - other.time_point_;
    }

private:
    TimePoint time_point_;
};

}

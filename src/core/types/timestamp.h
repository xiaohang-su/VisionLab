#pragma once

#include <chrono>
#include <cstdint>

namespace visionlab::core::types {

/// Unified timestamp type for VisionLab
/// Based on std::chrono::system_clock for standard C++ time representation
/// Provides consistent epoch semantics for real-world time values
class Timestamp {
public:
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    using TimePoint = Clock::time_point;

    /// Create a timestamp with current time
    static Timestamp now() {
        return Timestamp(Clock::now());
    }

    /// Create a timestamp with a specific timepoint
    explicit Timestamp(TimePoint tp = TimePoint{}) : time_point_(tp) {}

    /// Get the internal timepoint
    TimePoint time_point() const { return time_point_; }

    /// Get milliseconds since Unix epoch
    std::int64_t milliseconds_since_epoch() const {
        auto duration = time_point_.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    /// Get microseconds since Unix epoch
    std::int64_t microseconds_since_epoch() const {
        auto duration = time_point_.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    /// Compare timestamps
    bool operator==(const Timestamp& other) const { return time_point_ == other.time_point_; }
    bool operator!=(const Timestamp& other) const { return time_point_ != other.time_point_; }
    bool operator<(const Timestamp& other) const { return time_point_ < other.time_point_; }
    bool operator<=(const Timestamp& other) const { return time_point_ <= other.time_point_; }
    bool operator>(const Timestamp& other) const { return time_point_ > other.time_point_; }
    bool operator>=(const Timestamp& other) const { return time_point_ >= other.time_point_; }

    /// Calculate duration between two timestamps
    Duration operator-(const Timestamp& other) const {
        return time_point_ - other.time_point_;
    }

private:
    TimePoint time_point_;
};

}  // namespace visionlab::core::types

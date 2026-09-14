#pragma once

#include <cstdint>
#include <functional>

namespace visionlab::core::types {

class Id {
public:
    using ValueType = std::uint64_t;

    explicit Id(ValueType value = 0) : value_(value) {}

    ValueType value() const { return value_; }

    bool is_valid() const { return value_ != 0; }

    bool operator==(const Id& other) const { return value_ == other.value_; }
    bool operator!=(const Id& other) const { return value_ != other.value_; }
    bool operator<(const Id& other) const { return value_ < other.value_; }
    bool operator<=(const Id& other) const { return value_ <= other.value_; }
    bool operator>(const Id& other) const { return value_ > other.value_; }
    bool operator>=(const Id& other) const { return value_ >= other.value_; }

    friend struct std::hash<Id>;

private:
    ValueType value_;
};

}

template <>
struct std::hash<visionlab::core::types::Id> {
    std::size_t operator()(const visionlab::core::types::Id& id) const noexcept {
        return std::hash<visionlab::core::types::Id::ValueType>{}(id.value_);
    }
};

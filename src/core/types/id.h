#pragma once

#include <cstdint>
#include <functional>

namespace visionlab::core::types {

class ID {
public:
    using ValueType = std::uint64_t;

    explicit ID(ValueType value = 0) : value_(value) {}

    ValueType value() const { return value_; }

    bool is_valid() const { return value_ != 0; }

    bool operator==(const ID& other) const { return value_ == other.value_; }
    bool operator!=(const ID& other) const { return value_ != other.value_; }
    bool operator<(const ID& other) const { return value_ < other.value_; }
    bool operator<=(const ID& other) const { return value_ <= other.value_; }
    bool operator>(const ID& other) const { return value_ > other.value_; }
    bool operator>=(const ID& other) const { return value_ >= other.value_; }

    friend struct std::hash<ID>;

private:
    ValueType value_;
};

}

template <>
struct std::hash<visionlab::core::types::ID> {
    std::size_t operator()(const visionlab::core::types::ID& id) const noexcept {
        return std::hash<visionlab::core::types::ID::ValueType>{}(id.value_);
    }
};

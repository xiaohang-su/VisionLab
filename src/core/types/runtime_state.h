#pragma once

namespace visionlab::core::types {

enum class RuntimeState {
    Created,
    Initialized,
    Running,
    Stopped
};

inline const char* to_string(RuntimeState state) {
    switch (state) {
        case RuntimeState::Created:
            return "Created";
        case RuntimeState::Initialized:
            return "Initialized";
        case RuntimeState::Running:
            return "Running";
        case RuntimeState::Stopped:
            return "Stopped";
        default:
            return "Unknown";
    }
}

}

#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>

namespace visionlab::logging {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    explicit Logger(const std::string& log_file = "logs/visionlab.log");
    ~Logger();

    bool initialize();

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

    // Metrics (V0.9.1: measurement only, no optimization)
    double total_log_ms() const;
    std::uint64_t log_count() const;

private:
    void log(LogLevel level, const std::string& message);
    const char* level_to_string(LogLevel level) const;

    std::string log_file_;
    std::ofstream file_;
    mutable std::mutex mutex_;
    bool initialized_;

    double total_log_ms_ = 0.0;
    std::uint64_t log_count_ = 0;
};

}
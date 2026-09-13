#pragma once

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

private:
    void log(LogLevel level, const std::string& message);
    const char* level_to_string(LogLevel level) const;

    std::string log_file_;
    std::ofstream file_;
    std::mutex mutex_;
    bool initialized_;
};

}
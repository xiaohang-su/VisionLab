#include "logger.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace visionlab::logging {

Logger::Logger(const std::string& log_file)
    : log_file_(log_file),
      initialized_(false) {
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (file_.is_open()) {
        file_.close();
    }
}

bool Logger::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        return true;
    }

    try {
        const std::filesystem::path log_path(log_file_);
        const auto parent = log_path.parent_path();

        if (!parent.empty()) {
            std::filesystem::create_directories(parent);
        }

        file_.open(log_file_, std::ios::out | std::ios::app);

        if (!file_.is_open()) {
            return false;
        }

        initialized_ = true;
        return true;
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::Info, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::Error, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return;
    }

    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::ostringstream timestamp;
    timestamp << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");

    const std::string level_text = level_to_string(level);

    const std::string line =
        "[" + timestamp.str() + "] [" + level_text + "] " + message;

    std::cout << line << std::endl;

    if (file_.is_open()) {
        file_ << line << std::endl;
        file_.flush();
    }
}

const char* Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";

        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warning:
            return "WARNING";

        case LogLevel::Error:
            return "ERROR";

        default:
            return "UNKNOWN";
    }
}

}
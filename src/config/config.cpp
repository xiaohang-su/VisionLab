#include "config.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace visionlab::config {

bool Config::load(const std::string& file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        return false;
    }

    std::unordered_map<std::string, std::string> loaded_values;

    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        if (line[0] == '#') {
            continue;
        }

        const std::size_t separator = line.find('=');

        if (separator == std::string::npos) {
            continue;
        }

        const std::string key =
            trim(line.substr(0, separator));

        const std::string value =
            trim(line.substr(separator + 1));

        if (key.empty()) {
            continue;
        }

        loaded_values[key] = value;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_ = std::move(loaded_values);
    }

    return true;
}

bool Config::save(const std::string& file_path) const {
    try {
        const std::filesystem::path path(file_path);
        const auto parent = path.parent_path();

        if (!parent.empty()) {
            std::filesystem::create_directories(parent);
        }
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }

    std::ofstream file(file_path, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& [key, value] : values_) {
        file << key << "=" << value << '\n';
    }

    return file.good();
}

void Config::set(
    const std::string& key,
    const std::string& value
) {
    if (key.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    values_[key] = value;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);

    return values_.find(key) != values_.end();
}

std::string Config::get(
    const std::string& key,
    const std::string& default_value
) const {
    std::lock_guard<std::mutex> lock(mutex_);

    const auto iterator = values_.find(key);

    if (iterator == values_.end()) {
        return default_value;
    }

    return iterator->second;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(mutex_);

    values_.clear();
}

std::size_t Config::size() const {
    std::lock_guard<std::mutex> lock(mutex_);

    return values_.size();
}

std::string Config::trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";

    const std::size_t begin = value.find_first_not_of(whitespace);

    if (begin == std::string::npos) {
        return "";
    }

    const std::size_t end = value.find_last_not_of(whitespace);

    return value.substr(begin, end - begin + 1);
}

}
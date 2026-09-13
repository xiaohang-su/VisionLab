#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

namespace visionlab::config {

class Config {
public:
    Config() = default;
    ~Config() = default;

    bool load(const std::string& file_path);
    bool save(const std::string& file_path) const;

    void set(const std::string& key, const std::string& value);

    bool has(const std::string& key) const;

    std::string get(
        const std::string& key,
        const std::string& default_value = ""
    ) const;

    void clear();

    std::size_t size() const;

private:
    static std::string trim(const std::string& value);

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::string> values_;
};

}
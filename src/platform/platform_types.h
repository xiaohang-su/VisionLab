#pragma once

#include <string>
#include <cstdint>


namespace visionlab::platform {


enum class WeatherCondition
{
    Sunny,
    Cloudy,
    Rainy,
    Snowy,
    Unknown,
};


struct WeatherInfo
{
    WeatherCondition condition = WeatherCondition::Unknown;
    std::string city;
    std::string description;
    float temperature_c = 0.0f;
    float humidity = 0.0f;
    std::string update_time;
};


struct Announcement
{
    std::string title;
    std::string content;
    std::string date;
    bool urgent = false;
};


struct PopupMessage
{
    std::string title;
    std::string content;
    std::string button_text;
    bool show = false;
};


struct PlatformInfo
{
    std::string app_name = "VisionLab";
    std::string app_version = "1.1.0";
    std::string official_group = "QQ群: 待填写";
    std::string official_website = "https://github.com/xiaohang-su/VisionLab";
    std::string server_status = "正常";
    bool server_online = true;
};


}

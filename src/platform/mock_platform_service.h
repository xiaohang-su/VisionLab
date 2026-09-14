#pragma once

#include "platform_service.h"

#include <ctime>
#include <sstream>
#include <iomanip>


namespace visionlab::platform {


// Mock implementation for development and Linux builds.
class MockPlatformService : public PlatformService
{
public:

    const char* name() const override { return "MockPlatformService"; }

    WeatherInfo get_weather() override
    {
        WeatherInfo w;
        w.condition = WeatherCondition::Sunny;
        w.city = "广州";
        w.description = "晴";
        w.temperature_c = 26.0f;
        w.humidity = 65.0f;
        w.update_time = "2026-09-14 22:00";
        return w;
    }

    std::string get_time_string() override
    {
        std::time_t now = std::time(nullptr);
        std::tm* tm = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(tm, "%H:%M:%S");
        return oss.str();
    }

    std::vector<Announcement> get_announcements() override
    {
        std::vector<Announcement> list;

        Announcement a1;
        a1.title = "VisionLab 平台上线";
        a1.content = "VisionLab AI 视觉平台正式启动内测，欢迎体验。";
        a1.date = "2026-09-14";
        a1.urgent = true;
        list.push_back(a1);

        Announcement a2;
        a2.title = "模型支持说明";
        a2.content = "当前支持 YOLOv5/v8 ONNX 格式模型，更多格式持续适配中。";
        a2.date = "2026-09-14";
        list.push_back(a2);

        return list;
    }

    PopupMessage check_popup() override
    {
        PopupMessage p;
        p.show = false;
        return p;
    }

    const PlatformInfo& info() const override { return info_; }

private:

    PlatformInfo info_;
};


}

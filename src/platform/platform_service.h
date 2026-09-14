#pragma once

#include "platform_types.h"

#include <string>
#include <vector>


namespace visionlab::platform {


// Platform information service.
// Handles weather, announcements, popups, official group info.
// Future: real HTTP client fetches from server.
class PlatformService
{
public:

    virtual ~PlatformService() = default;

    virtual const char* name() const = 0;

    // Current weather (future: from weather API).
    virtual WeatherInfo get_weather() = 0;

    // Current time as display string.
    virtual std::string get_time_string() = 0;

    // Fetch announcement list from server.
    virtual std::vector<Announcement> get_announcements() = 0;

    // Check for popup messages (server-pushed notifications).
    virtual PopupMessage check_popup() = 0;

    // Static platform info.
    virtual const PlatformInfo& info() const = 0;
};


}

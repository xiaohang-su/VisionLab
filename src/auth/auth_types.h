#pragma once

#include <string>
#include <cstdint>


namespace visionlab::auth {


enum class AuthStatus
{
    NotLoggedIn,
    LoggingIn,
    Authenticated,
    Expired,
    Banned,
    NetworkError,
};


struct CardInfo
{
    std::string card_code;
    std::string user_id;
    std::string nickname;
    std::string expire_time;
    int remaining_days = 0;
    int level = 0;
};


struct LoginResult
{
    bool success = false;
    AuthStatus status = AuthStatus::NotLoggedIn;
    std::string message;
    CardInfo card;
};


struct UserSession
{
    bool active = false;
    std::string token;
    CardInfo card;
    int64_t login_timestamp = 0;
    int64_t last_heartbeat = 0;
};


}

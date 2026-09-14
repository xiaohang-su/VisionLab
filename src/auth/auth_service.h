#pragma once

#include "auth_types.h"

#include <string>


namespace visionlab::auth {


// Abstract authentication service.
// Future: WeiyanAuthService implements this against the 微验 server API.
class AuthService
{
public:

    virtual ~AuthService() = default;

    virtual const char* name() const = 0;

    // Verify a card key (卡密). Returns login result.
    virtual LoginResult verify_card(const std::string& card_code) = 0;

    // Periodic heartbeat to keep session alive.
    virtual bool heartbeat() = 0;

    // Fetch remote announcement text from server.
    virtual std::string get_announcement() = 0;

    // Current session state.
    virtual const UserSession& session() const = 0;

    virtual void logout() = 0;
};


}

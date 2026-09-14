#pragma once

#include "auth_service.h"


namespace visionlab::auth {


// Mock implementation for development and Linux builds.
// Returns fake data without any network calls.
class MockAuthService : public AuthService
{
public:

    const char* name() const override { return "MockAuthService"; }

    LoginResult verify_card(const std::string& card_code) override
    {
        LoginResult result;
        if (card_code.empty())
        {
            result.success = false;
            result.status = AuthStatus::NotLoggedIn;
            result.message = "请输入卡密";
            return result;
        }

        result.success = true;
        result.status = AuthStatus::Authenticated;
        result.message = "登录成功 (Mock)";
        result.card.card_code = card_code;
        result.card.user_id = "mock_user_001";
        result.card.nickname = "测试用户";
        result.card.expire_time = "2026-12-31";
        result.card.remaining_days = 365;
        result.card.level = 1;

        session_.active = true;
        session_.token = "mock_token_" + card_code;
        session_.card = result.card;

        return result;
    }

    bool heartbeat() override
    {
        if (!session_.active) return false;
        session_.last_heartbeat = 0;
        return true;
    }

    std::string get_announcement() override
    {
        return "VisionLab 平台公告：欢迎使用 VisionLab AI 视觉平台。当前为 Mock 认证模式，真实卡密验证将在对接微验服务器后启用。";
    }

    const UserSession& session() const override { return session_; }

    void logout() override
    {
        session_ = UserSession{};
    }

private:

    UserSession session_;
};


}

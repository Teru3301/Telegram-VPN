
#include "xui/models.hpp"


static xui::Config xui_config;

const xui::Config& xui::GetConfig()
{
    static bool initialized = false;
    if (!initialized)
    {
        const char* env_url = std::getenv("XUI_URL");
        xui_config.base_url = std::string(env_url);
        xui_config.timeuot = 5;
        xui_config.login = "admin";
        xui_config.password = "admin";
        Log("[3x-ui] init URL=" + xui_config.base_url);
    }
    return xui_config;
}


bool xui::SetCookie(const std::string& cookie)
{
    xui_config.cookie = cookie;
    return true;
}



#include "xui/models.hpp"
#include "loger.hpp"


namespace xui
{

static Config xui_config;

const Config& GetConfig()
{
    static bool initialized = false;
    if (!initialized)
    {
        const char* env_url = std::getenv("XUI_URL");
        xui_config.base_url = std::string(env_url);
        xui_config.timeout = 5;
        xui_config.login = "admin";
        xui_config.password = "admin";
        Log("[3x-ui] init URL=" + xui_config.base_url);
        initialized = true;
    }
    return xui_config;
}


bool SetCookie(const std::string& cookie)
{
    xui_config.cookie = cookie;
    return true;
}

}


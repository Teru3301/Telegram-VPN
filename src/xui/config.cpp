
#include "xui/models.hpp"
#include "loger.hpp"
#include "config.hpp"


namespace xui
{

static Config xui_config;

const Config& GetConfig()
{
    static bool initialized = false;
    if (!initialized)
    {
        xui_config.base_url = config::GetEnv("XUI_URL");
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


bool SetInboundId(int inbound_id)
{
    xui_config.inbound_id = inbound_id;
    return true;
}


}


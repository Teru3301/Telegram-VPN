
#include "xui/config.hpp"


static xui::Config xui_config;

const xui::Config& xui::GetConfig()
{
    static bool initialized = false;
    if (!initialized)
    {
        xui_config.base_url = "127.0.0.1:2053";
        xui_config.timeuot = 5;
    }
    return xui_config;
}


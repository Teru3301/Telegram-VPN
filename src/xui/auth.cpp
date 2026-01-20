
#include "xui/services.hpp"
#include "httplib.h"
#include "loger.hpp"


namespace xui
{

bool Service::IsAlive()
{
    auto cfg = GetConfig();
    httplib::Client cli = xui::utils::MakeClient(cfg);
    auto res = cli.Head("/");

    if (!res)
    {
        Log("[3x-ui] unavailable: connection failed");
        return false;
    }

    Log("[3x-ui] OK, reachable, http code = " + std::to_string(res->status));
    return true;
}


bool Service::Login()
{
    auto cfg = GetConfig();
    httplib::Client cli = xui::utils::MakeClient(cfg);

    std::string body =
        "username=" + cfg.login +
        "&password=" + cfg.password;

    auto res = cli.Post(
        "/login",
        body,
        "application/x-www-form-urlencoded"
    );

    if (!res) 
    {
        Log("[3x-ui] login failed: no response");
        return false;
    }

    if (res->status != 200 && res->status != 302)
    {
        Log("[3x-ui] login failed: http code = " + std::to_string(res->status));
        return false;
    }

    auto set_cookie = res->get_header_value("Set-Cookie");
    if (set_cookie.empty())
    {
        Log("[3x-ui] login failed: no Set-Cookie");
        return false;
    }

    SetCookie(set_cookie);

    Log("[3x-ui] login OK");
    return true;
}

}


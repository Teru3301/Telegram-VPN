
#include "xui/config.hpp"
#include "xui/utils.hpp"

#include "config.hpp"
#include "loger.hpp"


namespace xui::config
{


static models::XuiClient xui_client;


const models::XuiClient& GetXuiClient()
{
    static bool initialized = false;

    if (!initialized)
    {
        Log("[3x-ui] Init singleton");

        xui_client.base_url = ::config::GetEnv("XUI_URL");
        xui_client.timeout  = 5;
        xui_client.login    = ::config::GetEnv("XUI_LOGIN");
        xui_client.password = ::config::GetEnv("XUI_PASSWORD");

        Log("[3x-ui] init URL=" + xui_client.base_url);
        initialized = true;
    }

    return xui_client;
}


bool SetCookie(const std::string& cookie)
{
    xui_client.cookie = cookie;
    return true;
}


bool SetInboundId(int inbound_id)
{
    xui_client.inbound_id = inbound_id;
    return true;
}


bool IsAlive()
{
    auto cfg = GetXuiClient();
    auto cli = xui::utils::MakeClient(cfg);

    auto res = cli.Head("/");

    if (!res)
    {
        Log("[3x-ui] unavailable: connection failed");
        return false;
    }

    Log(
        "[3x-ui] OK, reachable, http code = " +
        std::to_string(res->status)
    );

    return true;
}


bool Login()
{
    auto cfg = GetXuiClient();
    auto cli = xui::utils::MakeClient(cfg);

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
        Log(
            "[3x-ui] login failed: http code = " +
            std::to_string(res->status)
        );
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


bool GetConnection()
{
    if (!IsAlive()) return false;
    if (!Login())   return false;

    bool connection_found = utils::TryFindConnection();

    if (!connection_found)
    {
        utils::CreateConnection(utils::GenerateRealityCert());
        connection_found = utils::TryFindConnection();
    }

    if (connection_found)
        Log("[3x-ui] [GetConnection] The connection was successfully found");
    else 
        Log("[3x-ui] [GetConnection] Unable to find connection");

    return connection_found;
}


}


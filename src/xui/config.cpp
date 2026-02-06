
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
        Log("[3x-ui] [config] Init singleton");

        std::string url = ::config::GetEnv("XUI_URL");

        // ---- defaults ----
        xui_client.path = "/";
        xui_client.timeout  = 5;
        xui_client.login    = ::config::GetEnv("XUI_LOGIN");
        xui_client.password = ::config::GetEnv("XUI_PASSWORD");

        // ---- scheme ----
        std::string scheme = "http://";
        if (url.rfind("http://", 0) == 0) {
            url.erase(0, 7);
        } else if (url.rfind("https://", 0) == 0) {
            scheme = "https://";
            url.erase(0, 8);
        }

        // ---- split path ----
        auto slash_pos = url.find('/');
        if (slash_pos != std::string::npos) {
            xui_client.path = url.substr(slash_pos);
            url = url.substr(0, slash_pos);
        }

        // ---- split host:port ----
        auto colon_pos = url.find(':');
        if (colon_pos != std::string::npos) {
            xui_client.host = url.substr(0, colon_pos);
            xui_client.port = url.substr(colon_pos + 1);
        } else {
            xui_client.host = url;
            xui_client.port = "80";
        }

        // ---- normalize path ----
        if (xui_client.path.empty() || xui_client.path[0] != '/') {
            xui_client.path = "/" + xui_client.path;
        }
        if (xui_client.path.size() > 1 && xui_client.path.back() == '/') {
            xui_client.path.pop_back();
        }

        // ---- base_url WITHOUT path ----
        std::string base_url = scheme + xui_client.host + ":" + xui_client.port + xui_client.path;

        Log("[3x-ui] [config] base_url = " + base_url);

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

    auto res = cli.Head(cfg.path + "/");

    if (!res)
    {
        Log("[3x-ui] [config] unavailable: connection failed");
        return false;
    }

    Log(
        "[3x-ui] [config] OK, reachable, http code = " +
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
        cfg.path + "/login",
        body,
        "application/x-www-form-urlencoded"
    );

    if (!res)
    {
        Log("[3x-ui] [config] login failed: no response");
        return false;
    }

    if (res->status != 200 && res->status != 302)
    {
        Log(
            "[3x-ui] [config] login failed: http code = " +
            std::to_string(res->status)
        );
        return false;
    }

    auto set_cookie = res->get_header_value("Set-Cookie");
    if (set_cookie.empty())
    {
        Log("[3x-ui] [config] login failed: no Set-Cookie");
        return false;
    }

    SetCookie(set_cookie);

    Log("[3x-ui] [config] login OK");
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
        Log("[3x-ui] [config] [GetConnection] The connection was successfully found");
    else 
        Log("[3x-ui] [config] [GetConnection] Unable to find connection");

    return connection_found;
}


}


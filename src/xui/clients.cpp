
#include "xui/services.hpp"
#include "nlohmann/json.hpp"
#include "loger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>


namespace xui
{

Client Service::CreateKey(uint64_t expiry_time, uint64_t tg_uid)
{
    Client vless_client = utils::GetClient("");
    
    if (!GetConnection())
    {
        Log("[3x-ui] no connection");
        return vless_client;
    }

    auto cfg = GetConfig();
    if (cfg.inbound_id <= 0)
    {
        Log("[3x-ui] bad inbound id");
        return vless_client;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream email;
    email << "tg_" << tg_uid << "_" << std::put_time(std::localtime(&now_time), "%Y.%m.%d-%H.%M.%S");

    nlohmann::json client = {
        {"id", utils::GenerateUUID()},
        {"email", email.str() },
        {"enable", true},
        {"expiryTime", now_time * 1000 + expiry_time}
    };

    nlohmann::json settings = {
        {"clients", nlohmann::json::array({client})}
    };

    nlohmann::json payload = {
        {"id", cfg.inbound_id},
        {"settings", settings.dump()}
    };

    httplib::Client cli = utils::MakeClient(cfg);
    httplib::Headers headers = {
        {"Cookie", cfg.cookie},
        {"Content-Type", "application/json"}
    };

    auto res = cli.Post(
        "/panel/api/inbounds/addClient",
        headers,
        payload.dump(),
        "application/json"
    );

    if (!res || res->status != 200)
    {
        Log("[3x-ui] bad res status");
        return vless_client;
    }

    auto j = nlohmann::json::parse(res->body);
    if (!j.contains("success") || !j["success"])
    {
        Log("[3x-ui] add client failed");
        Log("[3x-ui] json:\n" + j.dump());
        return {};
    }
    else if (j["success"] == true)
        Log("[3x-ui] add client success");

    return utils::GetClient(email.str());
}


bool DisableKey()
{
    return false;
}

bool EnableKey()
{
    return false;
}

bool GetTraffic()
{
    return false;
}

}


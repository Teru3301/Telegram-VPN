
#include "xui/services.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "loger.hpp"


namespace xui
{

bool Service::TryFindConnection()
{
    auto cfg = GetConfig();
    httplib::Client cli = xui::utils::MakeClient(cfg);

    httplib::Headers headers = {
        { "Cookie", cfg.cookie }
    };

    auto res = cli.Get("/panel/api/inbounds/list", headers);

    if (!res) {
        Log("[3x-ui] TryFindConnection: no response");
        return false;
    }

    if (res->status != 200) {
        Log("[3x-ui] TryFindConnection: http code = " + std::to_string(res->status));
        return false;
    }

    try {
        auto j = nlohmann::json::parse(res->body);

        if (!j.contains("success") || !j["success"]) {
            Log("[3x-ui] TryFindConnection: API returned success=false");
            return false;
        }

        if (!j.contains("obj") || !j["obj"].is_array()) {
            Log("[3x-ui] TryFindConnection: no obj array");
            return false;
        }

        for (const auto& inbound : j["obj"]) {
            if (inbound.contains("remark") &&
                inbound["remark"].is_string() &&
                inbound["remark"] == "bot")
            {
                Log("[3x-ui] Found existing inbound: remark=bot, id=" +
                    std::to_string(inbound["id"].get<int>()));
                return true;
            }
        }

    } catch (const std::exception& e) {
        Log(std::string("[3x-ui] TryFindConnection JSON error: ") + e.what());
        return false;
    }

    Log("[3x-ui] No existing connection found");
    return false;
}


bool Service::CreateConnection(const RealityCert& cert)
{
    auto cfg = GetConfig();
    httplib::Client cli = xui::utils::MakeClient(cfg);

    httplib::Headers headers = 
    {
        {"Cookie", cfg.cookie},
        {"Content-Type", "application/json"}
    };

    nlohmann::json settings_json = 
    {
        {"clients", nlohmann::json::array()},
        {"decryption", "none"}
    };

    nlohmann::json stream_settings_json = 
    {
        {"network","tcp"},
        {"security","reality"},
        {"realitySettings", 
        {
            {"show", false},
            {"dest", "www.google.com:443"},
            {"xver", 0},
            {"serverNames", nlohmann::json::array({"www.google.com"})},
            {"privateKey", cert.private_key},
            {"publicKey", cert.public_key},
            {"shortIds", cert.short_ids},
            {"settings", { {"publicKey", cert.public_key}, {"spiderX","/"} }}
        }}
    };

    nlohmann::json sniffing_json = 
    {
        {"enabled", true},
        {"destOverride", nlohmann::json::array({"http","tls"})}
    };

    nlohmann::json payload = 
    {
        {"remark","bot"},
        {"port",443},
        {"protocol","vless"},
        {"settings", settings_json.dump()},
        {"streamSettings", stream_settings_json.dump()},
        {"sniffing", sniffing_json.dump()},
        {"enable", true}
    };

    auto res = cli.Post("/panel/api/inbounds/add", headers, payload.dump(), "application/json");

    if (!res || res->status != 200)
    {
        Log("[3x-ui] CreateConnection failed");
        return false;
    }

    try
    {
        auto j = nlohmann::json::parse(res->body);
        if (!j.contains("success") || !j["success"])
        {
            Log("[3x-ui] CreateConnection failed: " + res->body);
            return false;
        }
    }
    catch (const std::exception& e)
    {
        Log("[3x-ui] CreateConnection JSON error: " + std::string(e.what()));
        return false;
    }

    Log("[3x-ui] Connection created successfully");
    return true;
}


bool Service::GetConnection()
{
    if (!IsAlive()) return false;
    if (!Login()) return false;
    if (!TryFindConnection())
    {
        CreateConnection(utils::GenerateRealityCert());
    }
    return TryFindConnection();
}

}


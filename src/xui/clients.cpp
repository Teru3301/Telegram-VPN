
#include "xui/services.hpp"
#include "nlohmann/json.hpp"
#include "loger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include "models.hpp"


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


//  Возвращает vless ключ и данные о нём по email
Key Service::GetVlessKey(const std::string& email)
{
    Key key{};
    key.email = email;

    if (!GetConnection())
    {
        Log("[3x-ui] no connection");
        return key;
    }

    auto cfg = GetConfig();
    if (cfg.inbound_id <= 0)
    {
        Log("[3x-ui] bad inbound id");
        return key;
    }

    httplib::Client cli = utils::MakeClient(cfg);
    httplib::Headers headers = {
        {"Cookie", cfg.cookie}
    };

    auto res = cli.Get(
        ("/panel/api/inbounds/get/" + std::to_string(cfg.inbound_id)).c_str(),
        headers
    );

    if (!res || res->status != 200)
    {
        Log("[3x-ui] failed to get inbound");
        return key;
    }

    auto j = nlohmann::json::parse(res->body);
    if (!j.contains("success") || !j["success"])
    {
        Log("[3x-ui] inbound request failed");
        return key;
    }

    auto inbound = j["obj"];
    auto settings = nlohmann::json::parse(inbound["settings"].get<std::string>());
    auto clients = settings["clients"];

    for (auto& c : clients)
    {
        if (c["email"] != email)
            continue;

        key.active   = c.value("enable", false);
        key.end_date = c.value("expiryTime", 0ULL);
        key.u_gb     = c.value("up", 0ULL)   / 1024.0 / 1024.0 / 1024.0;
        key.d_gb     = c.value("down", 0ULL) / 1024.0 / 1024.0 / 1024.0;

        // Reality stream settings
        auto stream = nlohmann::json::parse(
            inbound["streamSettings"].get<std::string>()
        );

        auto reality = stream["realitySettings"];

        std::string public_key = reality["publicKey"];
        std::string short_id   = reality["shortIds"][0];
        std::string sni        = reality["serverNames"][0];

        std::string host = "127.0.0.1";
        int port = inbound["port"];

        key.vless_uri = utils::BuildVlessKey(
            c["id"],
            host,
            port,
            public_key,
            short_id,
            sni,
            email
        );
        key.end_date /= 1000;

        return key;
    }

    Log("[3x-ui] client not found: " + email);
    return key;
}


}



#include "xui/keys.hpp"
#include "xui/utils.hpp"
#include "xui/config.hpp"
#include "models.hpp"
#include "loger.hpp"
#include "nlohmann/json.hpp"
#include "config.hpp"


namespace xui::keys
{


//  Создаёт ключ для подключения
models::Key CreateKey(int64_t expiry_time, int64_t tg_uid)
{
    Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + "]");
    models::Key key;

    if (!xui::config::GetConnection())
    {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] no connection");
        return key;
    }

    auto cfg = xui::config::GetXuiClient();
    if (cfg.inbound_id <= 0)
    {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] bad inbound id");
        return key;
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
        cfg.path + "/panel/api/inbounds/addClient",
        headers,
        payload.dump(),
        "application/json"
    );

    if (!res || res->status != 200)
    {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] bad res status");
        return key;
    }

    nlohmann::json j;
    try 
    {
        j = nlohmann::json::parse(res->body);
    } catch (const std::exception& e) {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] " + e.what());
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] raw body: '" + res->body + "'");
        return {};
    }

    if (!j.contains("success") || !j["success"])
    {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] add client failed");
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] json:\n" + j.dump());
        return {};
    }
    else if (j["success"] == true)
    {
        Log("[3x-ui] [CreateKey] [" + std::to_string(tg_uid) + 
            "] add client success");
    }

    return utils::GetClient(email.str());
}


//  Деактивирует ключ
bool DisableKey()
{
    return false;
}


//  Активирует ключ
bool EnableKey()
{
    return false;
}



// Возвращает vless ключ и данные о нём по email
models::Key GetVlessKey(const std::string& email)
{
    Log("[3x-ui] [GetVlessKey] client '" + email + "'");

    models::Key key{};
    key.email = email;
    key.active = false;

    if (!xui::config::GetConnection())
    {
        Log("[3x-ui] no connection");
        return key;
    }

    auto cfg = xui::config::GetXuiClient();
    if (cfg.inbound_id <= 0)
    {
        Log("[3x-ui] bad inbound id");
        return key;
    }

    httplib::Client cli = utils::MakeClient(cfg);
    httplib::Headers headers = {
        { "Cookie", cfg.cookie },
        { "Content-Type", "application/json" }
    };

    auto res = cli.Get(cfg.path + "/panel/api/inbounds/list", headers);
    if (!res || res->status != 200)
    {
        Log("[3x-ui] failed to get inbounds list");
        return key;
    }

    nlohmann::json j;
    try
    {
        j = nlohmann::json::parse(res->body);
    }
    catch (const std::exception& e)
    {
        Log(std::string("[3x-ui] json parse error: ") + e.what());
        return key;
    }

    if (!j.value("success", false) || !j.contains("obj"))
    {
        Log("[3x-ui] bad response structure");
        return key;
    }

    for (const auto& inbound : j["obj"])
    {
        if (inbound.value("id", -1) != cfg.inbound_id)
            continue;

        key.u = 0;
        key.d = 0;

        if (inbound.contains("clientStats") && inbound["clientStats"].is_array())
        {
            for (const auto& st : inbound["clientStats"])
            {
                if (st.value("email", "") == email)
                {
                    key.u = st.value("up", 0ULL);
                    key.d = st.value("down", 0ULL);
                    break;
                }
            }
        }

        auto settings = nlohmann::json::parse(
            inbound["settings"].get<std::string>()
        );

        if (!settings.contains("clients") || !settings["clients"].is_array())
            continue;

        for (const auto& c : settings["clients"])
        {
            if (c.value("email", "") != email)
                continue;

            key.active   = c.value("enable", false);
            key.end_date = c.value("expiryTime", 0ULL) / 1000;

            auto stream = nlohmann::json::parse(
                inbound["streamSettings"].get<std::string>()
            );

            auto reality = stream["realitySettings"];

            std::string public_key =
                reality["settings"]["publicKey"].get<std::string>();

            std::string short_id =
                reality["shortIds"].at(0).get<std::string>();

            std::string sni =
                reality["serverNames"].at(0).get<std::string>();

            std::string host = ::config::GetEnv("IP");
            int port = inbound["port"].get<int>();

            key.vless_uri = utils::BuildVlessKey(
                c["id"].get<std::string>(),
                host,
                port,
                public_key,
                short_id,
                sni,
                email
            );

            Log(
                "[3x-ui] [GetVlessKey] '" + email +
                "' up=" + std::to_string(key.u) +
                "B down=" + std::to_string(key.d) +
                "B exp=" + std::to_string(key.end_date)
            );

            return key;
        }
    }

    Log("[3x-ui] client not found: '" + email + "'");
    return key;
}


}


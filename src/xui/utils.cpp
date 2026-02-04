
#include "xui/utils.hpp"
#include "xui/config.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include "loger.hpp"
#include "models.hpp"
#include "nlohmann/json.hpp"
#include "config.hpp"


namespace xui::utils
{


std::string RandomHex(std::size_t len)
{
    static const char hex_chars[] = "0123456789abcdef";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 15);

    std::string result;
    result.reserve(len);

    for (std::size_t i = 0; i < len; ++i)
        result.push_back(hex_chars[dist(gen)]);

    return result;
}


std::vector<std::string> RandomShortIds()
{
    std::vector<int> lengths = { 2, 4, 6, 8, 10, 12, 14, 16 };

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::shuffle(lengths.begin(), lengths.end(), gen);

    std::vector<std::string> result;
    for (int len : lengths)
        result.push_back(RandomHex(len));

    return result;
}


models::RealityCert GenerateRealityCert()
{
    auto cfg = xui::config::GetXuiClient();
    httplib::Client cli = MakeClient(cfg);

    httplib::Headers headers =
    {
        { "Cookie", cfg.cookie },
        { "Content-Type", "application/json" }
    };

    auto res = cli.Post(
        "/server/getNewX25519Cert",
        headers,
        "",
        "application/json"
    );

    if (!res || res->status != 200)
    {
        Log("[3x-ui] Failed to get new X25519 cert");
        return {};
    }

    try
    {
        auto j = nlohmann::json::parse(res->body);

        if (!j.contains("success") || !j["success"])
        {
            Log("[3x-ui] getNewX25519Cert failed: " + res->body);
            return {};
        }

        models::RealityCert cert;
        cert.private_key = j["obj"]["privateKey"];
        cert.public_key  = j["obj"]["publicKey"];
        cert.short_ids   = RandomShortIds();

        return cert;
    }
    catch (const std::exception& e)
    {
        Log("[3x-ui] JSON parse error for cert: " + std::string(e.what()));
        return {};
    }
}


std::string GenerateUUID()
{
    static const char* hex = "0123456789abcdef";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 15);

    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (char& c : uuid)
    {
        if (c == 'x')
            c = hex[dist(gen)];
        else if (c == 'y')
            c = hex[(dist(gen) & 0x3) | 0x8];
    }

    return uuid;
}


std::string BuildVlessKey(
    const std::string& uuid,
    const std::string& host,
    int port,
    const std::string& public_key,
    const std::string& short_id,
    const std::string& sni,
    const std::string& email
)
{
    std::ostringstream ss;
    ss << "vless://" << uuid << "@"
       << host << ":" << port
       << "?type=tcp"
       << "&security=reality"
       << "&pbk=" << public_key
       << "&fp=chrome"
       << "&sni=" << sni
       << "&sid=" << short_id
       << "&spx=%2F"
       << "#" << email;

    return ss.str();
}


models::Key GetClient(const std::string& email)
{
    Log("[3x-ui] [GetClient] '" + email + "'");
    
    models::Key result{};
    result.active = false;

    auto cfg = xui::config::GetXuiClient();
    httplib::Client cli = MakeClient(cfg);

    httplib::Headers headers =
    {
        { "Cookie", cfg.cookie },
        { "Content-Type", "application/json" }
    };

    auto res = cli.Get("/panel/api/inbounds/list", headers);
    if (!res || res->status != 200)
    {
        Log("[3x-ui] [GetClient] '" + email + "' bad HTTP response");
        return result;
    }

    try
    {
        auto j = nlohmann::json::parse(res->body);

        if (!j.contains("success") || !j["success"])
        {
            Log("[3x-ui] [GetClient] '" + email + "' does not contain success");
            return result;
        }

        for (const auto& inbound : j["obj"])
        {
            if (inbound["id"].get<int>() != cfg.inbound_id) continue;
            int port = inbound["port"].get<int>();
            auto stream = nlohmann::json::parse(
                inbound["streamSettings"].get<std::string>()
            );
            auto reality = stream["realitySettings"];
            std::string public_key =
            reality["settings"]["publicKey"].get<std::string>();
            std::string sni        = reality["serverNames"][0];
            std::string short_id   = reality["shortIds"][0];
            auto settings = nlohmann::json::parse(
                inbound["settings"].get<std::string>()
            );

            for (const auto& client : settings["clients"])
            {
                if (client["email"] != email) continue;

                result.email       = email;
                result.end_date    = client["expiryTime"].get<int64_t>() / 1000;
                result.active      = client["enable"];

                result.vless_uri = BuildVlessKey(
                    client["id"],
                    ::config::GetEnv("IP"),
                    port,
                    public_key,
                    short_id,
                    sni,
                    email
                );

                return result;
            }
        }
    }
    catch (const std::exception& e)
    {
        Log(std::string("[3x-ui] [GetClient] '" + email + "' parse error: ") + e.what());
    }

    return result;
}


bool TryFindConnection()
{
    auto cfg = xui::config::GetXuiClient();
    httplib::Client cli = MakeClient(cfg);

    httplib::Headers headers =
    {
        { "Cookie", cfg.cookie }
    };

    auto res = cli.Get("/panel/api/inbounds/list", headers);
    if (!res || res->status != 200)
    {
        Log("[3x-ui] [TryFindConnection] Error. http status is missing or not 200");
        return false;
    }

    try
    {
        auto j = nlohmann::json::parse(res->body);

        if (!j.contains("success") || !j["success"])
        {
            Log("[3x-ui] [TryFindConnection] Error. ...");
            return false;
        }

        for (const auto& inbound : j["obj"])
        {
            if (inbound.contains("remark") &&
                inbound["remark"].is_string() &&
                inbound["remark"] == "bot")
            {
                xui::config::SetInboundId(inbound["id"].get<int>());
                Log("[3x-ui] [TryFindConnection] Success");
                return true;
            }
        }
    }
    catch (...)
    {
        Log("[3x-ui] [TryFindConnection] An exception was thrown");
        return false;
    }

    xui::config::SetInboundId(-1);
    Log("[3x-ui] [TryFindConnection] Some error");
    return false;
}


bool CreateConnection(const models::RealityCert& cert)
{
    auto cfg = xui::config::GetXuiClient();
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


httplib::Client MakeClient(const models::XuiClient& xui_client)
{
    httplib::Client cli(xui_client.base_url);
    cli.set_connection_timeout(xui_client.timeout);
    cli.set_read_timeout(xui_client.timeout);
    cli.set_write_timeout(xui_client.timeout);
    return cli;
}


}


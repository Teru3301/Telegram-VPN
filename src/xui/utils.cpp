
#include "xui/utils.hpp"
#include <random>
#include <algorithm>
#include "nlohmann/json.hpp"
#include <loger.hpp>


namespace xui
{

std::string utils::RandomHex(size_t len)
{
    static const char hex_chars[] = "0123456789abcdef";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::string s;
    s.reserve(len);

    for (size_t i = 0; i < len; ++i)
    {
        s += hex_chars[dis(gen)];
    }

    return s;
}

std::vector<std::string> utils::RandomShortIds()
{
    std::vector<int> lengths = {2, 4, 6, 8, 10, 12, 14, 16};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(lengths.begin(), lengths.end(), gen);

    std::vector<std::string> parts;

    for (int len : lengths)
    {
        parts.push_back(xui::utils::RandomHex(len));
    }

    return parts;
}

RealityCert utils::GenerateRealityCert()
{
    auto cfg = GetConfig();
    httplib::Client cli = xui::utils::MakeClient(cfg);

    httplib::Headers headers = {
        {"Cookie", cfg.cookie},
        {"Content-Type", "application/json"}
    };

    auto res = cli.Post("/server/getNewX25519Cert", headers, "", "application/json");
    if (!res || res->status != 200) {
        Log("[3x-ui] Failed to get new X25519 cert");
        return {};
    }

    try {
        auto j = nlohmann::json::parse(res->body);
        if (!j.contains("success") || !j["success"]) {
            Log("[3x-ui] getNewX25519Cert failed: " + res->body);
            return {};
        }

        RealityCert cert;
        cert.private_key= j["obj"]["privateKey"];
        cert.public_key = j["obj"]["publicKey"];
        cert.short_ids = xui::utils::RandomShortIds();
        return cert;

    } catch (const std::exception& e) {
        Log("[3x-ui] JSON parse error for cert: " + std::string(e.what()));
        return {};
    }
}


httplib::Client utils::MakeClient(const Config& cfg) {
    httplib::Client cli(cfg.base_url);
    cli.set_connection_timeout(cfg.timeout);
    cli.set_read_timeout(cfg.timeout);
    cli.set_write_timeout(cfg.timeout);
    return cli;
}


std::string utils::GenerateUUID()
{
    static const char* hex = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (char& c : uuid)
    {
        if (c == 'x') c = hex[dis(gen)];
        if (c == 'y') c = hex[(dis(gen) & 0x3) | 0x8];
    }
    return uuid;
}


std::string utils::BuildVlessKey(const std::string& uuid, const std::string& host, int port,
    const std::string& public_key, const std::string& short_id, const std::string& sni, const std::string& email)
{
    std::ostringstream ss;
    ss << "vless://" << uuid << "@" << host << ":" << port << "?type=tcp" << "&security=reality" << "&pbk=" << public_key 
        << "&fp=chrome" << "&sni=" << sni << "&sid=" << short_id << "&spx=%2F" << "#" << email;
    return ss.str();
}


Client utils::GetClient(const std::string& email)
{
    Client result{};
    result.enabled = false;

    auto cfg = GetConfig();

    httplib::Client cli = MakeClient(cfg);
    httplib::Headers headers = {
        {"Cookie", cfg.cookie},
        {"Content-Type", "application/json"}
    };

    auto res = cli.Get("/panel/api/inbounds/list", headers);
    if (!res || res->status != 200)
    {
        Log("[3x-ui] GetClient: bad HTTP response");
        return result;
    }

    try
    {
        auto j = nlohmann::json::parse(res->body);

        if (!j.contains("success") || !j["success"])
            return result;

        for (const auto& inbound : j["obj"])
        {
            if (inbound["id"].get<int>() != cfg.inbound_id)
                continue;

            int port = inbound["port"].get<int>();

            // --- streamSettings ---
            auto stream = nlohmann::json::parse(inbound["streamSettings"].get<std::string>());
            auto reality = stream["realitySettings"];

            std::string public_key = reality["publicKey"];
            std::string sni = reality["serverNames"][0];
            std::string short_id = reality["shortIds"][0];

            // --- clients ---
            auto settings = nlohmann::json::parse(inbound["settings"].get<std::string>());
            for (const auto& client : settings["clients"])
            {
                if (client["email"] != email)
                    continue;

                result.email = email;
                result.expiry_time = client["expiryTime"];
                result.expiry_time = result.expiry_time / 1000;
                Log("[3x-ui] expiryTime: " + std::to_string(result.expiry_time));
                result.enabled = client["enable"];

                std::string uuid = client["id"];

                result.vless_uri = BuildVlessKey(
                    uuid,
                    "127.0.0.1",
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
        Log(std::string("[3x-ui] GetClient parse error: ") + e.what());
    }

    return result;
}





}


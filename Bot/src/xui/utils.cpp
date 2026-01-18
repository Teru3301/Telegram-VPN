
#include "xui/utils.hpp"
#include <random>
#include <algorithm>
#include <nlohmann/json.hpp>
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


std::string utils::RandomShortIds()
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

    std::ostringstream oss;

    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (i > 0)
        {
            oss << ",";
        }
        oss << parts[i];
    }

    return oss.str();
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
        cert.short_ids.push_back(xui::utils::RandomShortIds());
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

}



#include "bot/config.hpp"
#include "loger.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include "models.hpp"


namespace bot::config
{


namespace
{
    models::Rule rule = models::Rule::Or;
    std::vector<models::Admin> admins;
    std::vector<models::Tarif> tarifs;
    std::vector<models::Manual> manuals;
}


static bool CreateDefaultConfig(const std::string& path)
{
    Log("[bot] [config] creating default config");

    nlohmann::json j =
    {
        { "rule", "or" },
        {
            "admins",
            {
                {
                    { "user_tag", "admin_username" },
                    { "user_id", 123456789 }
                }
            }
        },
        {
            "manuals",
            {
                {
                    { "title", "🖥️ Windows" },
                    { "link", "https://telegra.ph/Ispolzovanie-servisa-VPN-bot-na-operacionnoj-sisteme-Windows-02-07" }
                },
                {
                    { "title", "🐧 Linux" },
                    { "link", "https://telegra.ph/Ispolzovanie-servisa-VPN-bot-na-operacionnoj-sisteme-Linux-02-07" }
                },
                {
                    { "title", "📱Android" },
                    { "link", "https://telegra.ph/Ispolzovanie-servisa-VPN-bot-na-operacionnoj-sisteme-Android-02-07" }
                }
            }
        },
        {
            "tarifs",
            {
                {
                    { "title", "1 день - 10р." },
                    { "price", 10 },
                    { "time_in_ms", 86400000 }
                },
                {
                    { "title", "7 дней - 40р." },
                    { "price", 40 },
                    { "time_in_ms", 604800000 }
                },
                {
                    { "title", "1 месяц - 100р." },
                    { "price", 100 },
                    { "time_in_ms", 2592000000 }
                }
            }
        }
    };

    std::ofstream out(path);
    if (!out.is_open())
    {
        Log("[bot] [config] failed to create config file");
        return false;
    }

    out << j.dump(4);
    return true;
}


bool LoadConfig(const std::string& path)
{
    Log("[bot] [config] try load config");

    std::filesystem::create_directories("data");

    std::ifstream test(path);
    if (!test.good())
    {
        Log("[bot] [config] config not found");
        if (!CreateDefaultConfig(path)) return false;
        Log("[bot] [config] default config created");
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        Log("[bot] [config] failed to open config");
        return false;
    }

    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const nlohmann::json::exception& e)
    {
        Log(std::string("[bot] [config] json parse error: ") + e.what());
        return false;
    }

    try
    {
        // rule
        rule = (j.value("rule", "or") == "and")
            ? models::Rule::And
            : models::Rule::Or;

        // admins
        admins.clear();
        if (j.contains("admins") && j["admins"].is_array())
        {
            for (const auto& a : j["admins"])
            {
                if (!a.is_object()) continue;

                models::Admin admin;
                admin.user_tag = a.value("user_tag", "");
                admin.user_id  = a.value("user_id", int64_t{0});

                if (!admin.user_tag.empty() && admin.user_id != 0)
                    admins.push_back(std::move(admin));
            }
        }

        // tarifs
        tarifs.clear();
        if (j.contains("tarifs") && j["tarifs"].is_array())
        {
            for (const auto& t : j["tarifs"])
            {
                if (!t.is_object()) continue;

                models::Tarif tarif;
                tarif.title      = t.value("title", "");
                tarif.price      = t.value("price", uint64_t{0});
                tarif.time_in_ms = t.value("time_in_ms", uint64_t{0});

                if (!tarif.title.empty() && tarif.price > 0 && tarif.time_in_ms > 0)
                    tarifs.push_back(std::move(tarif));
            }
        }

        // manuals
        manuals.clear();
        if (j.contains("manuals") && j["manuals"].is_array())
        {
            for (const auto& m : j["manuals"])
            {
                if (!m.is_object()) continue;

                models::Manual manual;
                manual.title = m.value("title", "");
                manual.link  = m.value("link", "");

                if (!manual.title.empty() && !manual.link.empty())
                    manuals.push_back(std::move(manual));
            }
        }
    }
    catch (const std::exception& e)
    {
        Log(std::string("[bot] [config] parsing error: ") + e.what());
        return false;
    }

    Log("[bot] [config] config loaded successfully");
    return true;
}


//  Проверяет, является ли пользователь администратором
bool IsAdmin(const std::string& user_tag, int64_t user_id)
{
    for (const auto& admin : admins)
    {
        bool tag_match = !admin.user_tag.empty() && admin.user_tag == user_tag;
        bool id_match  = admin.user_id != 0 && admin.user_id == user_id;

        if (rule == models::Rule::Or)
        {
            if (tag_match || id_match)
            {
                Log("[bot] [config] [IsAdmin] user @" + user_tag + " [" + std::to_string(user_id) + "] is admin");
                return true;
            }
        }
        else 
        {
            if (tag_match && id_match)
            {
                Log("[bot] [config] [IsAdmin] user @" + user_tag + " [" + std::to_string(user_id) + "] is admin");
                return true;
            }
        }
    }

    Log("[bot] [config] [IsAdmin] user @" + user_tag + " [" + std::to_string(user_id) + "] is not admin");

    return false;
}


const std::vector<models::Manual>& GetManuals()
{
    return manuals;
}


}



#include "bot/config.hpp"
#include "loger.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>
#include <filesystem>


namespace bot::config
{


namespace
{
    enum class Rule
    {
        Or,
        And
    };

    struct Admin
    {
        std::string user_tag;
        int64_t     user_id;
    };

    Rule rule = Rule::Or;
    std::vector<Admin> admins;
}


//  Загружает список администраторов в кеш
bool LoadAdmins(const std::string& path)
{
    std::filesystem::create_directories("data");
    using json = nlohmann::json;

    admins.clear();
    rule = Rule::Or;

    std::ifstream file(path);

    if (!file.good())
    {
        json def = {
            {"rule", "or"},
            {"admins", {
                {
                    {"user_tag", "admin_username"},
                    {"user_id", 123456789}
                }
            }}
        };

        std::ofstream out(path);
        if (!out.good()) return false;

        out << def.dump(4);
        Log("[bot] [config] created new admins config");

        return true;
    }

    try
    {
        json data;
        file >> data;

        if (data.contains("rule") && data["rule"].is_string())
        {
            std::string r = data["rule"];
            rule = (r == "and") ? Rule::And : Rule::Or;
        }

        if (data.contains("admins") && data["admins"].is_array())
        {
            for (const auto& a : data["admins"])
            {
                Admin admin;
                admin.user_tag = a.value("user_tag", "");
                admin.user_id  = a.value("user_id", int64_t{0});

                if (!admin.user_tag.empty() || admin.user_id != 0)
                    admins.push_back(admin);
            }
        }
    }
    catch (const std::exception& e)
    {
        Log(std::string("[bot] [config] admin config parse error: ") + e.what());
        return false;
    }

    if (rule == Rule::Or) Log("[bot] is admin rule: or");
    else Log("[bot] [config] is admin rule: and");

    Log("[bot] [config] admins:");
    if (admins.empty())
        Log("[bot] [config] admins not set");
    for (const auto& admin : admins)
        Log("[bot] [config] - @" + admin.user_tag + " [" + std::to_string(admin.user_id) + "]");

    return true;
}


//  Проверяет, является ли пользователь администратором
bool IsAdmin(const std::string& user_tag, int64_t user_id)
{
    for (const auto& admin : admins)
    {
        bool tag_match = !admin.user_tag.empty() && admin.user_tag == user_tag;
        bool id_match  = admin.user_id != 0 && admin.user_id == user_id;

        if (rule == Rule::Or)
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


}


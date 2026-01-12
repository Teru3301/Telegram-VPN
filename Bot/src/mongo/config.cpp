
#include "mongo/config.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>


std::string GetTgBotToken()
{
    auto collection = Database::instance().getDB()["bot_settings"];
    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("key", "bot_token")
        )
    );

    if (!result) return ""; 

    return std::string(result->view()["value"].get_string().value);
}


void SetTgBotToken(const std::string& token)
{
    auto collection = Database::instance().getDB()["bot_settings"];
    collection.update_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("key", "bot_token")
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "$set",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("value", token)
                )
            )
        ),
        mongocxx::options::update{}.upsert(true)
    );
}


bool IsAdmin(int64_t user_id)
{
    auto db = Database::instance().getDB();
    mongocxx::collection admins_col = db["admins"];

    auto result = admins_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        )
    );

    return result.has_value();
}


bool AddAdmin(const std::string& admin_usertag)
{
    if (admin_usertag.empty())
        return false;

    auto db = Database::instance().getDB();
    mongocxx::collection admins_col = db["admins"];

    auto existing = admins_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("usertag", admin_usertag)
        )
    );

    if (existing)
        return false; 

    auto result = admins_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("usertag", admin_usertag)
        )
    );

    return static_cast<bool>(result);
}




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


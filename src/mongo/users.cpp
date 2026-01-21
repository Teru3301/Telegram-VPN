
#include "mongo/user_calls.hpp" 
#include <iostream>


UserState GetState(int64_t user_id)
{
    auto collection = Database::instance().getDB()["tg_users"];
    auto result = collection.find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("user_id", user_id)
                )
            );

    std::string state = std::string(result->view()["state"].get_string().value);
    return StringToState(state);
}


void SetState(int64_t user_id, const UserState& state)
{
    Database::instance().getDB()["tg_users"].update_one(
        bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("user_id", user_id)),
            bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("state", StateToString(state)))))
    );
}


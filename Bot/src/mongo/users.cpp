
#include "mongo/user_calls.hpp" 
#include <iostream>


bool CheckUserRegistration(const int id)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        )
    );

    return static_cast<bool>(result);
}


bool RegisterUser(const int id, const std::string& usertag)
{
    auto db = Database::instance().getDB();

    mongocxx::collection users_col  = db["tg_users"];
    mongocxx::collection admins_col = db["admins"];

    users_col.update_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "$setOnInsert",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("user_id", id),
                    bsoncxx::builder::basic::kvp("usertag", usertag),
                    bsoncxx::builder::basic::kvp("state", "Idle")
                )
            )
        ),
        mongocxx::options::update{}.upsert(true)
    );

    //  Добавление админки
    auto admin = admins_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("usertag", usertag)
        )
    );

    if (admin)
    {
        admins_col.update_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("usertag", usertag)
            ),
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$set",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("user_id", id)
                    )
                ),
                bsoncxx::builder::basic::kvp("$unset",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("usertag", "")
                    )
                )
            )
        );
    }

    return CheckUserRegistration(id);
}



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


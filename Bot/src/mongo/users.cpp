
#include "mongo/users.hpp" 


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
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.update_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "$setOnInsert",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("user_id", id),
                    bsoncxx::builder::basic::kvp("usertag", usertag),
                    bsoncxx::builder::basic::kvp("balance", 0.0),
                    bsoncxx::builder::basic::kvp("state", "Idle")
                )
            )
        ),
        mongocxx::options::update{}.upsert(true)
    );

    return CheckUserRegistration(id);
}


double CheckBalance(const int id)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        )
    );

    auto view = result->view();

    if (!view["balance"])
        return 0.0; 

    return view["balance"].get_double().value;
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


bool CheckPromo(const std::string& promo)
{
    return true;
}

bool UsePromo(int64_t user_id, const std::string& promo)
{
    return true;
}


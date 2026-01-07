
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


bool ReristerUser(const int id, const std::string& usertag)
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
                    bsoncxx::builder::basic::kvp("balance", 0.0)

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
        return 0.0; // поле balance отсутствует

    return view["balance"].get_double().value;
}




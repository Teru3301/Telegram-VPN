
#include "services/users.hpp"
#include "mongo/mongo.hpp"


namespace service::users 
{


//  Проверяет, зарегистрирован ли пользователь
bool CheckRegistration(const int64_t user_id)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        )
    );

    return static_cast<bool>(result);
}


//  Регистрирует нового пользователя
bool RegisterNew(const int64_t user_id, const std::string& user_tag)
{
    auto db = Database::instance().getDB();

    mongocxx::collection users_col  = db["tg_users"];

    users_col.update_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "$setOnInsert",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("user_id", user_id),
                    bsoncxx::builder::basic::kvp("usertag", user_tag),
                    bsoncxx::builder::basic::kvp("state", "Idle")
                )
            )
        ),
        mongocxx::options::update{}.upsert(true)
    );
    
    return CheckRegistration(user_id);
}


//  Проверяет по юзертегу, явлется ли пользователь администратором
bool IsAdmin(const std::string& user_tag)
{
    return false;
}


//  Проверяет, явлется ли пользователь администратором
bool IsAdmin(const int64_t user_id)
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


//  Назначает пользователя администратором по id
bool SetAdmin(const int64_t user_id)
{
    return false;
}


//  Назначает пользователя администратором по юзертегу 
bool SetAdmin(const std::string& user_tag)
{
    auto db = Database::instance().getDB();
    mongocxx::collection admins_col = db["admins"];

    auto existing = admins_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("usertag", user_tag)
        )
    );

    if (existing)
        return false; 

    auto result = admins_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("usertag", user_tag)
        )
    );

    return static_cast<bool>(result);
}


//  Возвращает текущее состояние бота для пользователя
UserState GetState(const int64_t user_id)
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


//  Задаёт состояни бота для пользователя
bool SetState(const int64_t user_id, const UserState& state)
{
    Database::instance().getDB()["tg_users"].update_one(
        bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("user_id", user_id)),
            bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("state", StateToString(state)))))
    );
    return true;
}


}



#include "services/users.hpp"
#include "mongo/core.hpp"
#include <mongocxx/client.hpp>
#include "bot/config.hpp"


namespace service::users 
{


//  Проверяет, зарегистрирован ли пользователь
bool CheckRegistration(const int64_t user_id)
{
    return mongo::Exist("tg_users", "user_id", user_id);
}


//  Регистрирует нового пользователя
bool RegisterNew(const int64_t user_id, const std::string& user_tag)
{
    return mongo::InsertIfNotExist(
        "tg_users",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("usertag", user_tag),
            bsoncxx::builder::basic::kvp("state", "Idle")
        )
    );
}


//  Возвращает текущее состояние бота для пользователя
UserState GetState(const int64_t user_id)
{
    std::string state = mongo::GetString(
        "tg_users",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "state"
    );

    return state.empty() ? UserState::Idle : StringToState(state);
}


//  Задаёт состояни бота для пользователя
bool SetState(const int64_t user_id, const UserState& state)
{
    return mongo::UpdateField(
        "tg_users",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "state",
        bsoncxx::types::b_string{StateToString(state)}
    );
}


//  проверяет, является ли пользователь администратором
bool IsAdmin(const std::string &user_tag, int64_t user_id)
{
    return bot::config::IsAdmin(user_tag, user_id);
}


}


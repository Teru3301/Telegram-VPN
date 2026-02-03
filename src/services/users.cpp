
#include "services/users.hpp"
#include "mongo/core.hpp"
#include <mongocxx/client.hpp>
#include "bot/config.hpp"
#include "loger.hpp"


namespace service::users
{

// Проверяет, зарегистрирован ли пользователь
bool CheckRegistration(const int64_t user_id)
{
    Log("[service] [users] [CheckRegistration] [TRY]   [" + std::to_string(user_id) +
        "] check registration");

    bool res = mongo::Exist("tg_users", "user_id", user_id);

    Log("[service] [users] [CheckRegistration] [" +
        std::string(res ? "OK]    " : "FAIL]  ") +
        "[" + std::to_string(user_id) + "] " +
        (res ? "user registered" : "user not registered"));

    return res;
}


// Регистрирует нового пользователя
bool RegisterNew(const int64_t user_id, const std::string& user_tag)
{
    Log("[service] [users] [RegisterNew] [TRY]   [" + std::to_string(user_id) +
        "] " + user_tag);

    bool res = mongo::InsertIfNotExist(
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

    Log("[service] [users] [RegisterNew] [" +
        std::string(res ? "OK]    " : "FAIL]  ") +
        "[" + std::to_string(user_id) + "] " +
        (res ? "user created" : "user already exists"));

    return res;
}


// Возвращает текущее состояние бота для пользователя
UserState GetState(const int64_t user_id)
{
    //Log("[service] [users] [GetState] [TRY]   [" + std::to_string(user_id) +
    //    "] get state");

    std::string state = mongo::GetString(
        "tg_users",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "state"
    );

    if (state.empty())
    {
    //    Log("[service] [users] [GetState] [FAIL]  [" + std::to_string(user_id) +
    //        "] state not found, fallback Idle");
        return UserState::Idle;
    }

    //Log("[service] [users] [GetState] [OK]    [" + std::to_string(user_id) +
    //    "] state=" + state);

    return StringToState(state);
}


// Задаёт состояние бота для пользователя
bool SetState(const int64_t user_id, const UserState& state)
{
    const std::string state_str = StateToString(state);

    //Log("[service] [users] [SetState] [TRY]   [" + std::to_string(user_id) +
    //    "] new_state=" + state_str);

    bool res = mongo::UpdateField(
        "tg_users",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "state",
        bsoncxx::types::b_string{state_str}
    );

    //Log("[service] [users] [SetState] [" +
    //    std::string(res ? "OK]    " : "FAIL]  ") +
    //    "[" + std::to_string(user_id) + "] state update");

    return res;
}


// Проверяет, является ли пользователь администратором
bool IsAdmin(const std::string& user_tag, int64_t user_id)
{
    Log("[service] [users] [IsAdmin] [TRY]   [" + std::to_string(user_id) +
        "] " + user_tag);

    bool res = bot::config::IsAdmin(user_tag, user_id);

    Log("[service] [users] [IsAdmin] [" +
        std::string(res ? "OK]    " : "FAIL]  ") +
        "[" + std::to_string(user_id) + "] " +
        (res ? "admin access granted" : "admin access denied"));

    return res;
}

}


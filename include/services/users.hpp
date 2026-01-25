
#pragma once

#include <stdint.h>
#include <string>
#include "bot/UserState.hpp"
#include "bot/config.hpp"


namespace service::users
{


bool CheckRegistration(const int64_t user_id);                          //  Проверяет, зарегистрирован ли пользователь
bool RegisterNew(const int64_t user_id, const std::string& user_tag);   //  Регистрирует нового пользователя
UserState GetState(const int64_t user_id);                              //  Возвращает текущее состояние бота для пользователя
bool SetState(const int64_t user_id, const UserState& state);           //  Задаёт состояни бота для пользователя
bool IsAdmin(const std::string &user_tag, int64_t user_id);             //  проверяет, является ли пользователь администратором

}


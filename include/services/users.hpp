
#pragma once

#include <stdint.h>
#include <string>
#include "bot/UserState.hpp"


namespace service::users
{


bool CheckRegistration(const int64_t user_id);                          //  Проверяет, зарегистрирован ли пользователь
bool RegisterNew(const int64_t user_id, const std::string& user_tag);   //  Регистрирует нового пользователя
bool IsAdmin(const int64_t user_id);                                    //  Проверяет по id, явлется ли пользователь администратором
bool IsAdmin(const std::string& user_tag);                              //  Проверяет по юзертегу, явлется ли пользователь администратором
bool SetAdmin(const int64_t user_id);                                   //  Назначает пользователя администратором по id
bool SetAdmin(const std::string& user_tag);                             //  Назначает пользователя администратором по юзертегу 
UserState GetState(const int64_t user_id);                              //  Возвращает текущее состояние бота для пользователя
bool SetState(const int64_t user_id, const UserState& state);           //  Задаёт состояни бота для пользователя
                                                                        //      (заменяется на id после /start)

}


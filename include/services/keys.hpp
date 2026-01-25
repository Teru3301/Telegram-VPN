
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "models.hpp"


namespace service::keys
{

bool CreateVless(const int64_t user_id, const int64_t expiry_time);     //  Создаёт vless ключ для пользователя
std::vector<models::Key> FindAll(int64_t user_id);                      //  Ищет все ключи пользователя
std::vector<models::Key> GetAll(int64_t user_id);                       //  TODO Возвращает все ключи пользователя с заполненной информацией
bool Find(const std::string& email);                                    //  Проверяет существование ключа


}


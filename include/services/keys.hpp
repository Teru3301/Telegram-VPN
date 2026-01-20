
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "models.hpp"


namespace service::keys
{

bool CreateVless(const int64_t user_id, const int64_t expiry_time);     //  Создаёт vless ключ для пользователя
std::vector<Key> FindAll(int64_t user_id);                              //  Ищет все ключи пользователя
bool Find(const std::string& email);                                    //  Проверяет существование ключа
Key Info(const std::string& email);                                     //  Собирает данные о ключе по названию


}


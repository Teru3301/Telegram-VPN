
#pragma once

#include <string>


namespace config
{


std::string GetEnv(const std::string& var);     //  Возвращает значение переменной окружения по её имени
bool CheckEnv(const std::string& var);          //  Проверяет существование переменной окружения


}



#include "config.hpp" 
#include "loger.hpp"


namespace config
{


//  Проверяет существование переменной окружения
bool CheckEnv(const std::string& var)
{
    const char* VARIABLE = std::getenv(var.c_str());
    if (!VARIABLE)
    {
        Log("[config] [CheckEnv] " + var + " not set");
        throw std::runtime_error("enviroment variable not set");
    }
    Log("[config] [CheckEnv] " + var + " is exists");
    return true; 
}


//  Возвращает значение переменной окружения по её имени
std::string GetEnv(const std::string& var)
{
    const char* VARIABLE = std::getenv(var.c_str());
    if (!VARIABLE)
    {
        Log("[config] [GetEnv] " + var + " not set");
        throw std::runtime_error("enviroment variable not set");
    }
    return VARIABLE;
}


}



#pragma once

#include "xui/utils.hpp"


class Key;  //  костыль (надеюсь временный)

namespace xui 
{

class Service 
{
public:
    static bool IsAlive();                                          //  Проверяет доступность 3x-ui
    static bool Login();                                            //  Логин
    static bool GetConnection();                                    //  Проверяет существование, создаёт и возвращает соединение 3x-ui
    static Client CreateKey(uint64_t expiry_time, uint64_t tg_uid); //  Создаёт ключ для подключения
    static bool DisableKey();                                       //  Деактивирует ключ
    static bool EnableKey();                                        //  Активирует ключ
    static Key GetVlessKey(const std::string& email);               //  Возвращает vless ключ и данные о нём по email

private:
    static bool TryFindConnection();                                //  Попытка найти 3x-ui соединение
    static bool CreateConnection(const RealityCert& cert);          //  Создание 3x-ui соединения

};

}


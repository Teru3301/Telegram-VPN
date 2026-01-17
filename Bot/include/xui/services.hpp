
#pragma once

#include <string>


namespace xui 
{

class Service 
{
public:
    static bool IsAlive();          //  Проверяет доступность 3x-ui
    static bool Login();            //  Логин
    static bool GetConnection();    //  Проверяет существование, создаёт и возвращает соединение 3x-ui
    bool CreateKey();               //  Создаёт ключ для подключения
    bool DisableKey();              //  Деактивирует ключ
    bool EnableKey();               //  Активирует ключ
    bool GetTrafic(const std::string& email);   //  Возвращает количество использованного трафика

private:
    bool TryFindConnection();       //  Попытка найти 3x-ui соединение
    bool CreateConnection();        //  Создание 3x-ui соединения

};

}


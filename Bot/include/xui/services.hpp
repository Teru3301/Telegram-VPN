
#pragma once

#include <string>


namespace xui 
{

class Service 
{
public:
    static bool IsAlive();
    bool Login();
    bool CreateConnection();
    bool CreateKey();
    bool DisableKey();
    bool EnableKey();
    bool GetTrafic(const std::string& email);
};

}


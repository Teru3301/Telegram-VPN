
#pragma once

#include <string>
#include "loger.hpp"


namespace xui
{

struct Config 
{
    std::string base_url;
    int timeuot;
    std::string login;
    std::string password;
    std::string cookie;
};

bool SetCookie(const std::string& cookie);
const Config& GetConfig();

}


#pragma once

#include <string>
#include <vector>


namespace xui
{

struct Config 
{
    std::string base_url;
    int timeout;
    std::string login;
    std::string password;
    std::string cookie;
};

struct RealityCert
{
    std::string private_key;
    std::string public_key;
    std::vector<std::string> short_ids;
};

bool SetCookie(const std::string& cookie);
const Config& GetConfig();

}


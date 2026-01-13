
#pragma once

#include <string> 


namespace xui
{
struct Config 
{
    std::string base_url;
    int timeuot;
};

const Config& GetConfig();
}


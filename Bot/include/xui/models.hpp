
#pragma once

#include <string>
#include <vector>
#include <cstdint>


namespace xui
{

struct RealityCert
{
    std::string private_key;
    std::string public_key;
    std::vector<std::string> short_ids;
};

struct Config 
{
    std::string base_url;
    int timeout;
    std::string login;
    std::string password;
    std::string cookie;
    int inbound_id;
};


struct Client
{
    std::string email;
    std::string vless_uri;
    uint64_t expiry_time;
    bool enabled = true;
};


bool SetCookie(const std::string& cookie);
bool SetInboundId(int inbound_id);
const Config& GetConfig();

}



#pragma once

#include <string>
#include <vector>
#include <cstdint>


namespace models
{


struct Key
{
    std::string vless_uri;
    std::string email;
    int64_t end_date;
    double d_gb = 0.0;
    double u_gb = 0.0;
    bool active;
};


struct XuiClient
{
    std::string base_url;
    std::string login;
    std::string password;
    std::string cookie;
    int timeout;
    int inbound_id;
};


struct Client
{
    std::string email;
    std::string vless_uri;
    int64_t expiry_time;
    bool enabled = true;
};


struct RealityCert
{
    std::string private_key;
    std::string public_key;
    std::vector<std::string> short_ids;
};


}


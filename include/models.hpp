
#pragma once

#include <string>


struct Key
{
    std::string vless_uri;
    std::string email;
    int64_t end_date;
    double d_gb = 0.0;
    double u_gb = 0.0;
    bool active;
};


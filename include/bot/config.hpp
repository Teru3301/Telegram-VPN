
#pragma once

#include <string>
#include <cstdint>


namespace bot::config
{


bool LoadAdmins(const std::string& path);
bool IsAdmin(const std::string& user_tag, int64_t user_id);


}



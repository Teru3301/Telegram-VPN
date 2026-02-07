
#pragma once

#include <string>
#include <cstdint>
#include "models.hpp"


namespace bot::config
{


bool IsAdmin(const std::string& user_tag, int64_t user_id);
bool LoadConfig(const std::string& path);
const std::vector<models::Manual>& GetManuals();


}



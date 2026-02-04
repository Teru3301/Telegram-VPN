
#pragma once

#include "models.hpp"


namespace xui::keys
{


models::Key CreateKey(int64_t expiry_time, int64_t tg_uid);

bool DisableKey(const std::string& email);
bool EnableKey(const std::string& email);

models::Key GetVlessKey(const std::string& email);


}


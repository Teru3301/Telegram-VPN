
#pragma once

#include "mongo/mongo.hpp"
#include <string> 


std::string GetTgBotToken();
void SetTgBotToken(const std::string& token);

bool IsAdmin(int64_t user_id);
bool AddAdmin(const std::string& admin_usertag);


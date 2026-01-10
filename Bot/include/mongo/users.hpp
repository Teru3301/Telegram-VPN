
#pragma once

#include "mongo/mongo.hpp"
#include "bot/UserState.hpp"


bool CheckUserRegistration(const int id);
bool RegisterUser(const int id, const std::string& usertag);
double CheckBalance(const int id);

UserState GetState (int64_t user_id);
void SetState (int64_t user_id, const UserState& state);

bool CheckPromo(const std::string& promo);
bool UsePromo(int64_t user_id, const std::string& promo);


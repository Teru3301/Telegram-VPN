
#pragma once

#include "mongo/mongo.hpp"
#include "bot/UserState.hpp"


bool CheckUserRegistration(const int id);
bool RegisterUser(const int id, const std::string& usertag);

UserState GetState (int64_t user_id);
void SetState (int64_t user_id, const UserState& state);

bool CheckPromo(const std::string& promo);
bool UsePromo(int64_t user_id, const std::string& promo);

struct Key
{
    std::string key;
    int64_t end_date;
    double d_gb = 0.0;
    double u_gb = 0.0;
};
std::vector<Key> FindKeys(const int user_id);

void CreatePromoDraft(int64_t user_id);
void SetPromoDraftEndDate(int64_t user_id, int64_t seconds);
void SetPromoDraftBonus(int64_t user_id, int64_t seconds);
void SetPromoDraftUses(int64_t user_id, int64_t uses);
void SetPromoDraftPromo(int64_t user_id, std::string uses);
bool CreatePromo(int64_t user_id);


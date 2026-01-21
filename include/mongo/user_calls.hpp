
#pragma once

#include "mongo/mongo.hpp"
#include "bot/UserState.hpp"
#include "models.hpp"


UserState GetState (int64_t user_id);
void SetState (int64_t user_id, const UserState& state);

bool UsePromo(int64_t user_id, const std::string& promo);

std::vector<Key> FindKeys(const int user_id);

void CreatePromoDraft(int64_t user_id);
void SetPromoDraftEndDate(int64_t user_id, int64_t seconds);
void SetPromoDraftBonus(int64_t user_id, int64_t seconds);
void SetPromoDraftUses(int64_t user_id, int64_t uses);
void SetPromoDraftPromo(int64_t user_id, std::string uses);
bool CreatePromo(int64_t user_id);

namespace mongo
{


bool AddVlessKey(const Key& key, const int64_t user_id);
std::vector<Key> FindAll(const int64_t user_id);
Key Find(const std::string& email);


}




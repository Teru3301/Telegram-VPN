
#pragma once

#include "mongo/mongo.hpp"
#include "models.hpp"


//  TODO
//  DELETE
namespace mongo
{


bool AddVlessKey(const Key& key, const int64_t user_id);
std::vector<Key> FindAll(const int64_t user_id);


}




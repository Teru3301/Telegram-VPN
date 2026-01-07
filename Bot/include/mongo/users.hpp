
#pragma once

#include "mongo/mongo.hpp"


bool CheckUserRegistration(const int id);
bool ReristerUser(const int id, const std::string& usertag);
double CheckBalance(const int id);


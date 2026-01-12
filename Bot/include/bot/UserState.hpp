
#pragma once

#include <string>


enum class UserState {
    Idle,
    WaitPromo,
    CreatePromoAviableUses,
    CreatePromoEnterPromocode,
    Payment
};


std::string StateToString (const UserState& state);
UserState StringToState (const std::string& state);


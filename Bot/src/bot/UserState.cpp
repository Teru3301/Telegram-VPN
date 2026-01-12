
#include "bot/UserState.hpp" 


std::string StateToString(const UserState& state)
{
    switch (state) {
        case UserState::Idle: return "Idle";
        case UserState::WaitPromo: return "WaitPromo";
        case UserState::Payment: return "Payment";
        case UserState::CreatePromoAviableUses: return "CreatePromoAviableUses";
        case UserState::CreatePromoEnterPromocode: return "CreatePromoEnterPromocode";
    }
    return "Idle";
}

UserState StringToState (const std::string& state)
{
    if (state == "CreatePromoAviableUses") return UserState::CreatePromoAviableUses;
    if (state == "CreatePromoEnterPromocode") return UserState::CreatePromoEnterPromocode;
    if (state == "WaitPromo") return UserState::WaitPromo;
    if (state == "Payment") return UserState::Payment;
    return UserState::Idle;
}


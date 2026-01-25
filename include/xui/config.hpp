
#pragma once

#include <string>
#include "models.hpp"


namespace xui::config
{


bool IsAlive();
bool Login();
bool GetConnection();

bool SetCookie(const std::string& cookie);
bool SetInboundId(int inbound_id);

const models::XuiClient& GetXuiClient();


}


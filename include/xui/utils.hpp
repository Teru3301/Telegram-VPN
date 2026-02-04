
#pragma once

#include <string>
#include <vector>
#include <httplib.h>
#include "models.hpp"


namespace xui::utils
{


std::string RandomHex(std::size_t len);
std::vector<std::string> RandomShortIds();

models::RealityCert GenerateRealityCert();
std::string GenerateUUID();

std::string BuildVlessKey(
    const std::string& uuid,
    const std::string& host,
    int port,
    const std::string& public_key,
    const std::string& short_id,
    const std::string& sni,
    const std::string& email
);

models::Key GetClient(const std::string& email);

bool TryFindConnection();
bool CreateConnection(const models::RealityCert& cert);

httplib::Client MakeClient(const models::XuiClient& xui_client);


}


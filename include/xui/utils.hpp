
#pragma once

#include "xui/models.hpp"
#include <httplib.h>


namespace xui::utils
{

std::string RandomHex(size_t len);
std::vector<std::string> RandomShortIds();
RealityCert GenerateRealityCert();
httplib::Client MakeClient(const Config& cfg);
std::string GenerateUUID();
std::string BuildVlessKey(const std::string& uuid, const std::string& host, int port, const std::string& public_key, const std::string& short_id, const std::string& sni, const std::string& email);
Client GetClient(const std::string& email);

}


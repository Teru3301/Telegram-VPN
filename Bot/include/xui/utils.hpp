
#pragma once

#include "xui/models.hpp"
#include <httplib.h>


namespace xui::utils
{

std::string RandomHex(size_t len);
std::string RandomShortIds();
RealityCert GenerateRealityCert();
httplib::Client MakeClient(const Config& cfg);

}


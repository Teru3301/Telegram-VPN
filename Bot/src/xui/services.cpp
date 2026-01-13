
#include "xui/services.hpp" 
#include <curl/curl.h>
#include "loger.hpp"
#include "xui/config.hpp" 


bool xui::Service::IsAlive()
{
    auto cfg = GetConfig();
    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, cfg.base_url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);        // HEAD запрос
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, cfg.timeuot);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    if (res == CURLE_OK)
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        Log(std::string("[3x-ui] unavailable: ") + curl_easy_strerror(res));
        return false;
    }

    Log("[3x-ui] reachable, http code = " + std::to_string(http_code));
    return true;
}


bool xui::Service::Login()
{
    return false;
}


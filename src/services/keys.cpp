
#include "services/keys.hpp"
#include "xui/services.hpp"
#include "mongo/user_calls.hpp"


namespace service::keys
{


//  Создаёт vless ключ для пользователя
bool CreateVless(const int64_t user_id, const int64_t expiry_time)
{
    auto xui_client = xui::Service::CreateKey(expiry_time, user_id);
    Key key;
    key.active = xui_client.enabled;
    key.end_date = xui_client.expiry_time;
    key.email = xui_client.email;
    key.vless_uri = xui_client.vless_uri;

    mongo::AddVlessKey(key, user_id);

    return key.active;
}


//  Ищет все ключи пользователя
std::vector<Key> FindAll(int64_t user_id)
{
    return mongo::FindAll(user_id);
}


//  Проверяет существование ключа
bool Find(const std::string& email)
{
    Key key = mongo::Find(email);
    return key.active;
}


//  Собирает данные о ключе по названию
Key Info(const std::string& email)
{
    Key key = mongo::Find(email);
    
    // TODO
    // запросы к 3x-ui 
    // - трафик
    // - активен или нет

    return key;
}


}


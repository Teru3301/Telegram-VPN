
#include "services/keys.hpp"
#include "xui/services.hpp"
#include "mongo/core.hpp"


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

    return mongo::InsertIfNotExist(
        "vless_keys",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("email", key.email)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("email", key.email)
        )
    );
}


//  Возвращает все ключи пользователя с заполненной информацией
std::vector<Key> GetAll(int64_t user_id)
{
    std::vector<std::string> emails = mongo::GetAllString(
        "vless_keys",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "email"
    );

    std::vector<Key> keys;
    for (auto email : emails)
        keys.push_back(xui::Service::GetVlessKey(email));

    return keys;
}


//  Проверяет существование ключа
bool Find(const std::string& email)
{
    return mongo::Exist("vless_keys", "email", email);
}


}


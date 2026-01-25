
#include "services/keys.hpp"
#include "mongo/core.hpp"
#include "xui/keys.hpp"


namespace service::keys
{


//  Создаёт vless ключ для пользователя
bool CreateVless(const int64_t user_id, const int64_t expiry_time)
{
    auto xui_client = xui::keys::CreateKey(expiry_time, user_id);
    models::Key key;
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
std::vector<models::Key> GetAll(int64_t user_id)
{
    std::vector<std::string> emails = mongo::GetAllString(
        "vless_keys",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "email"
    );

    std::vector<models::Key> keys;
    for (auto email : emails)
        keys.push_back(xui::keys::GetVlessKey(email));

    return keys;
}


//  Проверяет существование ключа
bool Find(const std::string& email)
{
    return mongo::Exist("vless_keys", "email", email);
}


}


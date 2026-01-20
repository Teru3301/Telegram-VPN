
#include "mongo/user_calls.hpp"


namespace mongo
{


bool AddVlessKey(const Key& key, const int64_t user_id)
{
    try {
        auto collection = Database::instance().getDB()["vless_keys"];
            
        // Используем basic::make_document (более современный способ)
        auto doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("vless_uri", key.vless_uri),
            bsoncxx::builder::basic::kvp("email", key.email),
            bsoncxx::builder::basic::kvp("end_date", 
                bsoncxx::types::b_date{
                    std::chrono::system_clock::from_time_t(key.end_date)
                }
            ),
            bsoncxx::builder::basic::kvp("d_gb", key.d_gb),
            bsoncxx::builder::basic::kvp("u_gb", key.u_gb),
            bsoncxx::builder::basic::kvp("active", key.active),
            bsoncxx::builder::basic::kvp("created_at", 
                bsoncxx::types::b_date{std::chrono::system_clock::now()}
            )
        );
        
        auto result = collection.insert_one(std::move(doc));
        return result && result->result().inserted_count() == 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding vless key: " << e.what() << std::endl;
        return false;
    }
}


std::vector<Key> FindAll(const int64_t user_id)
{
    std::vector<Key> keys;
    
    try {
        auto collection = Database::instance().getDB()["vless_keys"];
        
        // Создаем фильтр для поиска по user_id
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        );
        
        // Выполняем запрос
        auto cursor = collection.find(filter.view());
        
        // Обрабатываем результаты
        for (auto&& doc : cursor) {
            Key key;
            
            // Извлекаем поля из документа
            auto user_id_field = doc["user_id"];
            auto vless_uri_field = doc["vless_uri"];
            auto email_field = doc["email"];
            auto end_date_field = doc["end_date"];
            auto d_gb_field = doc["d_gb"];
            auto u_gb_field = doc["u_gb"];
            auto active_field = doc["active"];
            
            // Проверяем существование полей и преобразуем значения
            if (vless_uri_field && vless_uri_field.type() == bsoncxx::type::k_string) {
                key.vless_uri = vless_uri_field.get_string().value;
            }
            
            if (email_field && email_field.type() == bsoncxx::type::k_string) {
                key.email = email_field.get_string().value;
            }
            
            if (end_date_field && end_date_field.type() == bsoncxx::type::k_date) {
                auto date_value = end_date_field.get_date().value;
                // Конвертируем время из chrono в time_t
                auto time_point = std::chrono::system_clock::time_point(
                    std::chrono::milliseconds(date_value)
                );
                key.end_date = std::chrono::system_clock::to_time_t(time_point);
            }
            
            if (d_gb_field && d_gb_field.type() == bsoncxx::type::k_double) {
                key.d_gb = d_gb_field.get_double().value;
            }
            
            if (u_gb_field && u_gb_field.type() == bsoncxx::type::k_double) {
                key.u_gb = u_gb_field.get_double().value;
            }
            
            if (active_field && active_field.type() == bsoncxx::type::k_bool) {
                key.active = active_field.get_bool().value;
            }
            
            keys.push_back(key);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error finding vless keys: " << e.what() << std::endl;
    }
    
    return keys;
}


Key Find(const std::string& email)
{
    Key key;
    
    try {
        auto collection = Database::instance().getDB()["vless_keys"];
        
        // Создаем фильтр для поиска по email
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("email", email)
        );
        
        // Выполняем поиск одного документа
        auto result = collection.find_one(filter.view());
        
        if (result) {
            auto doc = *result;
            
            // Извлекаем поля из документа
            auto user_id_field = doc["user_id"];
            auto vless_uri_field = doc["vless_uri"];
            auto email_field = doc["email"];
            auto end_date_field = doc["end_date"];
            auto d_gb_field = doc["d_gb"];
            auto u_gb_field = doc["u_gb"];
            auto active_field = doc["active"];
            
            // Проверяем существование полей и преобразуем значения
            if (vless_uri_field && vless_uri_field.type() == bsoncxx::type::k_string) {
                key.vless_uri = vless_uri_field.get_string().value;
            }
            
            if (email_field && email_field.type() == bsoncxx::type::k_string) {
                key.email = email_field.get_string().value;
            }
            
            if (end_date_field && end_date_field.type() == bsoncxx::type::k_date) {
                auto date_value = end_date_field.get_date().value;
                // Конвертируем время из chrono в time_t
                auto time_point = std::chrono::system_clock::time_point(
                    std::chrono::milliseconds(date_value)
                );
                key.end_date = std::chrono::system_clock::to_time_t(time_point);
            }
            
            if (d_gb_field && d_gb_field.type() == bsoncxx::type::k_double) {
                key.d_gb = d_gb_field.get_double().value;
            }
            
            if (u_gb_field && u_gb_field.type() == bsoncxx::type::k_double) {
                key.u_gb = u_gb_field.get_double().value;
            }
            
            if (active_field && active_field.type() == bsoncxx::type::k_bool) {
                key.active = active_field.get_bool().value;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error finding vless key by email: " << e.what() << std::endl;
    }
    
    return key;
}


}


#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <stdexcept>
#include <string>


class Database {
public:
    Database() = default;

    static void init(const std::string& uri)
    {
        if (!instance_ptr_)
            instance_ptr_ = new Database(uri);
    }

    static Database& instance()
    {
        if (!instance_ptr_)
            throw std::runtime_error("Database not initialized. Call Database::init first.");
        return *instance_ptr_;
    }

    mongocxx::database& getDB() { return db_; }
    
    // Явное преобразование std::string_view в std::string
    std::string getDatabaseName() { return std::string(db_.name()); }

private:
    Database(const std::string& uri)
        : client_(mongocxx::uri{uri})
    {
        // Получаем имя БД из URI
        std::string dbname = mongocxx::uri{uri}.database();
        if (dbname.empty()) {
            throw std::runtime_error("Database name not specified in URI");
        }
        db_ = client_[dbname];
    }

    mongocxx::client client_;
    mongocxx::database db_;
    static mongocxx::instance mongo_instance_;
    static Database* instance_ptr_;
};


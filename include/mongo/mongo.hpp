
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

    static void init(const std::string& uri, const std::string& dbname)
    {
        if (!instance_ptr_)
            instance_ptr_ = new Database(uri, dbname);
    }

    static Database& instance()
    {
        if (!instance_ptr_)
            throw std::runtime_error("Database not initialized. Call Database::init first.");
        return *instance_ptr_;
    }

    mongocxx::database& getDB() { return db_; }

private:
    Database(const std::string& uri, const std::string& dbname)
        : client_(mongocxx::uri{uri}),
          db_(client_[dbname])
    {}

    mongocxx::client client_;
    mongocxx::database db_;
    static mongocxx::instance mongo_instance_;
    static Database* instance_ptr_;
};


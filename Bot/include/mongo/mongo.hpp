
#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>


class Database {
public:
    Database();
    static Database& instance();

    mongocxx::database& getDB() { return db_; }  // <-- добавляем getter

private:
    mongocxx::client client_;
    mongocxx::database db_;
    static mongocxx::instance mongo_instance_;
};


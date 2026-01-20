
#include "mongo/mongo.hpp"


mongocxx::instance Database::mongo_instance_{};


Database::Database() : client_(mongocxx::uri{std::getenv("MONGO_URI")}), db_(client_["vpn_bot_db"])
{
}


Database& Database::instance()
{
    static Database instance;
    return instance;
}



#include "mongo/mongo.hpp"

mongocxx::instance Database::mongo_instance_{};
Database* Database::instance_ptr_ = nullptr;


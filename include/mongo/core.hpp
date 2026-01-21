
#pragma once

#include <string>
#include <bsoncxx/builder/basic/document.hpp>


namespace mongo
{


bool Exist(const std::string& document, const std::string& field, int64_t id);
bool Exist(const std::string& document, const std::string& field, const std::string& id);
bool Upsert(const std::string& document, const bsoncxx::document::view& filter, const bsoncxx::document::view& data);
bool InsertIfNotExist(const std::string& document, const bsoncxx::document::view& filter, const bsoncxx::document::view& data);


}


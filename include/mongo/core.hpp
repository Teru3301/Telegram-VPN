
#pragma once

#include <string>
#include <bsoncxx/builder/basic/document.hpp>
#include <vector>


namespace mongo
{


bool Exist(
        const std::string& document, 
        const std::string& field, 
        int64_t id
        );
bool Exist(
        const std::string& document, 
        const std::string& field, 
        const std::string& id
        );
bool Upsert(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const bsoncxx::document::view& data
        );
bool InsertIfNotExist(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const bsoncxx::document::view& data
        );
bool UpdateField(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const std::string& field, 
        const bsoncxx::types::bson_value::value& value
        );
std::string GetString(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const std::string& field
        );
std::vector<std::string> GetAllString(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const std::string& field
        );
int64_t GetInt64(
        const std::string& document, 
        const bsoncxx::document::view& filter, 
        const std::string& field
        );
bool Delete(
        const std::string& document, 
        const bsoncxx::document::view& filter
        );


}


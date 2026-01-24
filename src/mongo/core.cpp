
#include "mongo/core.hpp"
#include "mongo/mongo.hpp"
#include <mongocxx/collection.hpp>


namespace mongo
{


bool Exist(const std::string& document, const std::string& field, int64_t id)
{
    try
    {
        auto collection = Database::instance().getDB()[document];

        auto result = collection.find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp(field, id)
            )
        );

        return static_cast<bool>(result);
    }
    catch (const std::exception&)
    {
        return false;
    }
}


bool Exist(const std::string& document, const std::string& field, const std::string& id)
{
    try
    {
        auto collection = Database::instance().getDB()[document];

        auto result = collection.find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp(field, id)
            )
        );

        return static_cast<bool>(result);
    }
    catch (const std::exception&)
    {
        return false;
    }
}


bool Upsert(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const bsoncxx::document::view& data
)
{
    try
    {
        auto col = Database::instance().getDB()[document];

        auto result = col.update_one(
            filter,
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$setOnInsert", data)
            ),
            mongocxx::options::update{}.upsert(true)
        );

        return result.has_value();
    }
    catch (...)
    {
        return false;
    }
}


bool InsertIfNotExist(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const bsoncxx::document::view& data
)
{
    try
    {
        auto col = Database::instance().getDB()[document];

        auto result = col.update_one(
            filter,
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$setOnInsert", data)
            ),
            mongocxx::options::update{}.upsert(true)
        );

        if (!result)
            return false;

        return result->upserted_id().has_value();
    }
    catch (...)
    {
        return false;
    }
}


bool UpdateField(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const std::string& field,
    const bsoncxx::types::bson_value::value& value
)
{
    try
    {
        auto col = Database::instance().getDB()[document];

        auto result = col.update_one(
            filter,
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp(
                    "$set",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp(field, value)
                    )
                )
            )
        );

        return result && result->modified_count() > 0;
    }
    catch (...)
    {
        return false;
    }
}


std::string GetString(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const std::string& field
)
{
    try
    {
        auto col = Database::instance().getDB()[document];
        auto result = col.find_one(filter);

        if (!result)
            return {};

        auto view = result->view();

        auto element = view[field];
        if (!element || element.type() != bsoncxx::type::k_string)
            return {};

        return std::string(element.get_string().value);
    }
    catch (...)
    {
        return {};
    }
}


std::vector<std::string> GetAllString(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const std::string& field
)
{
    std::vector<std::string> result;

    try
    {
        auto col = Database::instance().getDB()[document];
        auto cursor = col.find(filter);

        for (const auto& doc : cursor)
        {
            auto element = doc[field];
            if (!element || element.type() != bsoncxx::type::k_string)
                continue;

            result.emplace_back(element.get_string().value);
        }
    }
    catch (...)
    {
        return {};
    }

    return result;
}


int64_t GetInt64(
    const std::string& document,
    const bsoncxx::document::view& filter,
    const std::string& field
)
{
    try
    {
        auto col = Database::instance().getDB()[document];
        auto result = col.find_one(filter);

        if (!result)
            return 0;

        auto view = result->view();
        auto element = view[field];

        if (!element || element.type() != bsoncxx::type::k_int64)
            return 0;

        return element.get_int64().value;
    }
    catch (...)
    {
        return 0;
    }
}


bool Delete(
    const std::string& document,
    const bsoncxx::document::view& filter
)
{
    try
    {
        auto col = Database::instance().getDB()[document];

        auto result = col.delete_one(filter);

        if (!result)
            return false;

        return result->deleted_count() > 0;
    }
    catch (...)
    {
        return false;
    }
}


}


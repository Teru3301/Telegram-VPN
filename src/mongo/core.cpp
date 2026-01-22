
#include "mongo/core.hpp"
#include "mongo/mongo.hpp"


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


}


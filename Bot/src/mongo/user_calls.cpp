
#include "mongo/user_calls.hpp"


std::vector<Key> FindKeys(const int user_id)
{
    std::vector<Key> result;

    mongocxx::collection collection =
        Database::instance().getDB()["vpn_keys"];

    auto cursor = collection.find(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("active", true)
        )
    );

    for (const auto& doc : cursor)
    {
        auto view = doc;

        if (!view["key"] || !view["end_date"])
            continue;

        if (view["key"].type() != bsoncxx::type::k_string)
            continue;

        if (view["end_date"].type() != bsoncxx::type::k_int64)
            continue;

        Key key;
        key.key = std::string(
            view["key"].get_string().value
        );
        key.end_date = view["end_date"].get_int64().value;

        result.push_back(std::move(key));
    }

    return result;
}



#include "mongo/users.hpp" 


bool CheckUserRegistration(const int id)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        )
    );

    return static_cast<bool>(result);
}


bool RegisterUser(const int id, const std::string& usertag)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.update_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "$setOnInsert",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("user_id", id),
                    bsoncxx::builder::basic::kvp("usertag", usertag),
                    bsoncxx::builder::basic::kvp("balance", 0.0),
                    bsoncxx::builder::basic::kvp("state", "Idle")
                )
            )
        ),
        mongocxx::options::update{}.upsert(true)
    );

    return CheckUserRegistration(id);
}


double CheckBalance(const int id)
{
    auto collection = Database::instance().getDB()["tg_users"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", id)
        )
    );

    auto view = result->view();

    if (!view["balance"])
        return 0.0; 

    return view["balance"].get_double().value;
}


UserState GetState(int64_t user_id)
{
    auto collection = Database::instance().getDB()["tg_users"];
    auto result = collection.find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("user_id", user_id)
                )
            );

    std::string state = std::string(result->view()["state"].get_string().value);
    return StringToState(state);
}

void SetState(int64_t user_id, const UserState& state)
{
    Database::instance().getDB()["tg_users"].update_one(
        bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("user_id", user_id)),
            bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("state", StateToString(state)))))
    );
}


bool CheckPromo(const std::string& promo)
{
    mongocxx::collection collection =
        Database::instance().getDB()["promo"];

    auto result = collection.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        )
    );

    if (!result)
        return false;


    auto view = result->view();

    if (!view["aviable_uses"] || !view["end_date"])
        return false;

    if (view["aviable_uses"].type() != bsoncxx::type::k_int32 &&
        view["aviable_uses"].type() != bsoncxx::type::k_int64)
        return false;

    if (view["end_date"].type() != bsoncxx::type::k_int32 &&
        view["end_date"].type() != bsoncxx::type::k_int64)
        return false;

    int64_t aviable_uses =
        view["aviable_uses"].get_int64().value;

    int64_t end_date =
        view["end_date"].get_int64().value;

    int64_t now = static_cast<int64_t>(std::time(nullptr));

    if (aviable_uses <= 0)
        return false;

    if (now >= end_date)
        return false;

    return true;
}


bool UsePromo(int64_t user_id, const std::string& promo)
{
    auto db = Database::instance().getDB();

    mongocxx::collection promo_col = db["promo"];
    mongocxx::collection keys_col  = db["vpn_keys"];

    auto promo_doc = promo_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        )
    );

    if (!promo_doc)
        return false;

    auto promo_view = promo_doc->view();

    if (!promo_view["bonus"] || !promo_view["bonus_period"])
        return false;

    std::string bonus_key =
        std::string(promo_view["bonus"].get_string().value);

    int64_t bonus_period =
        promo_view["bonus_period"].get_int64().value;

    auto existing_key = keys_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("key", bonus_key)
        )
    );

    if (existing_key)
        return false; 

    auto update_result = promo_col.find_one_and_update(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo),
            bsoncxx::builder::basic::kvp("aviable_uses",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$gt", 0)
                )
            )
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$inc",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("aviable_uses", -1)
                )
            )
        )
    );

    if (!update_result)
        return false; 

    int64_t now = static_cast<int64_t>(std::time(nullptr));
    int64_t end_date = now + bonus_period;

    keys_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("key", bonus_key),
            bsoncxx::builder::basic::kvp("end_date", end_date),
            bsoncxx::builder::basic::kvp("active", true)
        )
    );

    return true;
}


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


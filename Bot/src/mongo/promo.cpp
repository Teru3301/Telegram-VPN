
#include "mongo/user_calls.hpp"


bool CheckPromo(const std::string& promo)
{
    auto col = Database::instance().getDB()["promo"];

    return static_cast<bool>(
        col.find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("promo", promo)
            )
        )
    );
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

    if (!promo_view["aviable_uses"] || !promo_view["end_date"])
        return false;

    if ((promo_view["aviable_uses"].type() != bsoncxx::type::k_int32 &&
         promo_view["aviable_uses"].type() != bsoncxx::type::k_int64) ||
        (promo_view["end_date"].type() != bsoncxx::type::k_int32 &&
         promo_view["end_date"].type() != bsoncxx::type::k_int64))
        return false;

    int64_t aviable_uses =
        promo_view["aviable_uses"].get_int64().value;

    int64_t promo_end_date =
        promo_view["end_date"].get_int64().value;

    int64_t now = static_cast<int64_t>(std::time(nullptr));

    if (aviable_uses <= 0)
        return false;

    if (now >= promo_end_date)
        return false;

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

    keys_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("key", bonus_key),
            bsoncxx::builder::basic::kvp("end_date", now + bonus_period),
            bsoncxx::builder::basic::kvp("active", true)
        )
    );

    return true;
}


void CreatePromoDraft(int64_t user_id)
{
    auto col = Database::instance().getDB()["promo_drafts"];

    col.update_one(
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)),
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ))),
        mongocxx::options::update{}.upsert(true)
    );
}


void SetPromoDraftEndDate(int64_t user_id, int64_t seconds)
{
    auto col = Database::instance().getDB()["promo_drafts"];
    col.update_one(
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)),
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("end_date", seconds)
        )))
    );
}


void SetPromoDraftBonus(int64_t user_id, int64_t seconds)
{
    auto col = Database::instance().getDB()["promo_drafts"];
    col.update_one(
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)),
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("bonus_period", seconds)
        )))
    );
}


void SetPromoDraftUses(int64_t user_id, int64_t uses)
{
    auto col = Database::instance().getDB()["promo_drafts"];
    col.update_one(
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)),
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("aviable_uses", uses)
        )))
    );
}


void SetPromoDraftPromo(int64_t user_id, std::string promo)
{
    auto col = Database::instance().getDB()["promo_drafts"];
    col.update_one(
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)),
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        )))
    );
}


bool CreatePromo(int64_t user_id)
{
    auto db = Database::instance().getDB();

    mongocxx::collection drafts_col = db["promo_drafts"];
    mongocxx::collection promo_col  = db["promo"];

    auto draft = drafts_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        )
    );

    if (!draft)
        return false;

    auto view = draft->view();

    if (!view["promo"] ||
        !view["aviable_uses"] ||
        !view["end_date"] ||
        !view["bonus_period"])
        return false;

    std::string promo =
        std::string(view["promo"].get_string().value);

    int64_t aviable_uses =
        view["aviable_uses"].get_int64().value;

    int64_t end_date =
        view["end_date"].get_int64().value;

    int64_t bonus_period =
        view["bonus_period"].get_int64().value;

    auto existing = promo_col.find_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        )
    );

    if (existing)
        return false;

    std::string bonus_key = "test";

    auto result = promo_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo),
            bsoncxx::builder::basic::kvp("aviable_uses", aviable_uses),
            bsoncxx::builder::basic::kvp("end_date", end_date),
            bsoncxx::builder::basic::kvp("bonus_period", bonus_period),
            bsoncxx::builder::basic::kvp("bonus", bonus_key)
        )
    );

    if (!result)
        return false;

    drafts_col.delete_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        )
    );

    return true;
}


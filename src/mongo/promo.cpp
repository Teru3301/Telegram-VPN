
#include "mongo/user_calls.hpp"
#include "xui/services.hpp"
#include "loger.hpp"
#include <chrono>


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
    {
        Log("[mongo] No uses left");
        return false;
    }

    if (now >= promo_end_date)
    {
        Log("[mongo] The promo code has expired");
        return false;
    }

    if (!promo_view["bonus_period"])
        return false;

    int64_t bonus_period =
        promo_view["bonus_period"].get_int64().value;
    auto vless_client = xui::Service::CreateKey(bonus_period, user_id);
   
    auto update_result = promo_col.find_one_and_update(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo),
            bsoncxx::builder::basic::kvp("aviable_uses",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$gt", 0)
                )
            ),
            // пользователь ещё НЕ использовал промокод
            bsoncxx::builder::basic::kvp("used_by",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$ne", user_id)
                )
            )
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$inc",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("aviable_uses", -1)
                )
            ),
            bsoncxx::builder::basic::kvp("$push",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("used_by", user_id)
                )
            )
        )
    );

    if (!update_result)
    {
        Log("[mongo] Promo already used by this user or no uses left");
        return false;
    }

    keys_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("key", vless_client.vless_uri),
            bsoncxx::builder::basic::kvp("email", vless_client.email),
            bsoncxx::builder::basic::kvp("active", vless_client.enabled),
            bsoncxx::builder::basic::kvp("end_date", static_cast<int64_t>(vless_client.expiry_time))
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

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream email;

    auto result = promo_col.insert_one(
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo),
            bsoncxx::builder::basic::kvp("aviable_uses", aviable_uses),
            bsoncxx::builder::basic::kvp("end_date", end_date + now_time * 1000),
            bsoncxx::builder::basic::kvp("bonus_period", bonus_period)
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


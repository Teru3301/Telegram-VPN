
#include "services/promo.hpp"
#include "services/keys.hpp"
#include "mongo/user_calls.hpp"
#include "mongo/core.hpp"
#include "xui/services.hpp"


namespace service::promo
{


//  Ищет сопадение по названию
bool Find(const std::string& promo)
{
    return mongo::Exist("promo", "promo", promo);
}


//  Проверяет, можно ли использовать промокод
bool Check(const std::string& promo)
{
    if (!mongo::Exist("promo", "promo", promo)) return false;

    auto filter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("promo", promo)
    );

    int64_t expires_at = mongo::GetInt64("promo", filter, "end_date");
    int64_t uses       = mongo::GetInt64("promo", filter, "uses");

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (expires_at > 0 && expires_at < now) return false; 
    if (uses <= 0) return false;

    return true; 
}


//  Использовать промокод
bool Use(int64_t user_id, const std::string& promo)
{
    if (!Check(promo)) return false;

    int64_t uses = mongo::GetInt64(
        "promo",
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("promo", promo)),
        "uses"
    );

    int64_t bonus_period = mongo::GetInt64(
        "promo",
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("promo", promo)),
        "bonus_period"
    );

    bool updated = mongo::UpdateField(
        "promo",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        ),
        "uses",
        bsoncxx::types::b_int64{uses - 1}
    );

    if (!updated) return false;

    return service::keys::CreateVless(user_id, bonus_period);
}


//  Создаёт шаблон промокода
bool CreateDraft(int64_t user_id)
{
    return mongo::InsertIfNotExist(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("promo", ""),
            bsoncxx::builder::basic::kvp("expires_at", int64_t{0}),
            bsoncxx::builder::basic::kvp("bonus_period", int64_t{0}),
            bsoncxx::builder::basic::kvp("uses", int64_t{0})
        )
    );
}


//  Удаляет шаблон промокода
bool DeleteDraft(int64_t user_id)
{
    return mongo::Delete(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        )
    );
}


//  Задаёт дату окончания действия промокода
bool SetDraftEndDate(int64_t user_id, int64_t time)
{
    return mongo::UpdateField(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "promo_period",
        bsoncxx::types::b_int64{time}
    );
}


//  Задаёт время действия бонусного кода после активации промокода
bool SetDraftBonus(int64_t user_id, int64_t time)
{
    return mongo::UpdateField(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "bonus_period",
        bsoncxx::types::b_int64{time}
    );
}


//  Задаёт количество использований прокомода
bool SetDraftUses(int64_t user_id, int64_t uses)
{
    return mongo::UpdateField(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "uses",
        bsoncxx::types::b_int64{uses}
    );
}


//  Задаёт название промокода
bool SetDraftPromo(int64_t user_id, const std::string& promo)
{
    if (Find(promo)) return false;

    return mongo::UpdateField(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "promo",
        bsoncxx::types::b_string{promo}
    );
}


//  Создаёт промокод с параметрами из шаблона
bool CreateByDraft(int64_t user_id)
{
    if (!mongo::Exist("promo_drafts", "user_id", user_id)) return false;

    auto filter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("user_id", user_id)
    );

    std::string promo = mongo::GetString("promo_drafts", filter, "promo");
    int64_t promo_period = mongo::GetInt64("promo_drafts", filter, "promo_period");
    int64_t bonus_period = mongo::GetInt64("promo_drafts", filter, "bonus_period");
    int64_t uses = mongo::GetInt64("promo_drafts", filter, "uses");

    if (promo.empty() || promo_period <= 0 || uses <= 0) return false;

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    DeleteDraft(user_id);

    return mongo::InsertIfNotExist(
        "promo",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("promo", promo)
        ),
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("promo", promo),
            bsoncxx::builder::basic::kvp("expires_at", promo_period + now_time),
            bsoncxx::builder::basic::kvp("bonus_period", bonus_period),
            bsoncxx::builder::basic::kvp("uses", uses),
            bsoncxx::builder::basic::kvp("create_date", now_time)
        )
    );
}


}


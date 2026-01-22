
#include "services/promo.hpp"
#include "mongo/user_calls.hpp"
#include "mongo/core.hpp"


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
    //  TODO
    return mongo::Exist("promo", "promo", promo);
}


//  Использовать промокод
bool Use(int64_t user_id, const std::string& promo)
{
    return UsePromo(user_id, promo);
}


//  Создаёт шаблон промокода
bool CreateDraft(int64_t user_id)
{
    CreatePromoDraft(user_id);
    return true;
}


//  Удаляет шаблон промокода
bool DeleteDraft(int64_t user_id)
{
    return false;
}


//  Задаёт дату окончания действия промокода
bool SetDraftEndDate(int64_t user_id, int64_t time)
{
    return mongo::UpdateField(
        "promo_drafts",
        bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id)
        ),
        "end_date",
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
    return CreatePromo(user_id);
}


}


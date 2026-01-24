
#pragma once

#include <stdint.h>
#include <string>


namespace service::promo
{


bool Find(const std::string& promo);                            //  Ищет сопадение по названию
bool Check(const std::string& promo);                           //  Проверяет, можно ли использовать промокод
bool Use(int64_t user_id, const std::string& promo);            //  Использовать промокод
bool CreateDraft(int64_t user_id);                              //  Создаёт шаблон промокода
bool DeleteDraft(int64_t user_id);                              //  Удаляет шаблон промокода
bool SetDraftEndDate(int64_t user_id, int64_t time);            //  Задаёт дату окончания действия промокода
bool SetDraftBonus(int64_t user_id, int64_t time);              //  Задаёт время действия бонусного кода после активации промокода
bool SetDraftUses(int64_t user_id, int64_t uses);               //  Залаёт количество использований прокомода
bool SetDraftPromo(int64_t user_id, const std::string& promo);  //  Задаёт название промокода
bool CreateByDraft(int64_t user_id);                            //  Создаёт промокод с параметрами из шаблона


}


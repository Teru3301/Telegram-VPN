
#include "bot/commands.hpp"
#include "services/users.hpp"
#include "services/promo.hpp"


MessageView PromoEndDate(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
   
    service::promo::CreateDraft(user_id);

    std::ostringstream text;
    text << "Выберите срок жизни промокода:";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("1 час", "promo_end:3600000")});
    keyboard->inlineKeyboard.push_back({MakeButton("1 день", "promo_end:86400000")});
    keyboard->inlineKeyboard.push_back({MakeButton("7 дней", "promo_end:604800000")});
    keyboard->inlineKeyboard.push_back({MakeButton("30 дней", "promo_end:2592000000")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return {
        text.str(),
        keyboard
    };
}


class CreatePromoCommand : public Command {
public:
    std::string name() const override {
        return "/create_promo";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Create promo step-1 command");
        Log(msg);
        
        auto view = PromoEndDate(msg->from->id);
        
        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard,
            "HTML"
        );
    }
};


class CreatePromoCallback : public Callback {
public:
    std::string name() const override {
        return "create_promo";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Create promo step-1 callback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = PromoEndDate(query->from->id);

            bot.getApi().editMessageText(
                view.text,
                query->message->chat->id,
                query->message->messageId,
                "",
                "HTML",
                nullptr,
                view.keyboard,
                {}
            );
        } 
        catch (...) 
        {
            Log("Кнопка устарела");
            bot.getApi().sendMessage (
                query->message->chat->id,
                "Кнопка устарела. Используйте /create_promo"
            );
        }
    }
};


MessageView PromoBonus(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::ostringstream text;
    text
        << "Выберите время действия ключа после активации:";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("1 час", "bonus:3600000")});
    keyboard->inlineKeyboard.push_back({MakeButton("1 день", "bonus:86400000")});
    keyboard->inlineKeyboard.push_back({MakeButton("7 дней", "bonus:604800000")});
    keyboard->inlineKeyboard.push_back({MakeButton("30 дней", "bonus:2592000000")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return {
        text.str(),
        keyboard
    };
}


class CreatePromoBonusCallback : public Callback {
public:
    std::string name() const override {
        return "promo_end";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Create promo step-2 callback");

        std::string data = query->data; 
        if (data.find(":") != std::string::npos)
        {
            Log(data);
            auto seconds = std::stoll(data.substr(data.find(":") + 1));
            service::promo::SetDraftEndDate(query->from->id, seconds);
        }

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = PromoBonus(query->from->id);

            bot.getApi().editMessageText(
                view.text,
                query->message->chat->id,
                query->message->messageId,
                "",
                "HTML",
                nullptr,
                view.keyboard,
                {}
            );
        } 
        catch (...) 
        {
            Log("Кнопка устарела");
            bot.getApi().sendMessage (
                query->message->chat->id,
                "Кнопка устарела. Используйте /create_promo"
            );
        }
    }
};

MessageView PromoPromo(int64_t user_id)
{
    service::users::SetState(user_id, UserState::CreatePromoAviableUses);
    
    std::ostringstream text;
    text
        << "Введите количество использований промокода (целое положительное число)";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return {
        text.str(),
        keyboard
    };
}


class CreatePromoPromoCallback : public Callback {
public:
    std::string name() const override {
        return "bonus";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Create promo step-3 callback");
        
        std::string data = query->data; 
        if (data.find(":") != std::string::npos)
        {
            Log(data);
            auto seconds = std::stoll(data.substr(data.find(":") + 1));
            service::promo::SetDraftBonus(query->from->id, seconds);
        }

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = PromoPromo(query->from->id);

            bot.getApi().editMessageText(
                view.text,
                query->message->chat->id,
                query->message->messageId,
                "",
                "HTML",
                nullptr,
                view.keyboard,
                {}
            );
        } 
        catch (...) 
        {
            Log("Кнопка устарела");
            bot.getApi().sendMessage (
                query->message->chat->id,
                "Кнопка устарела. Используйте /create_promo"
            );
        }
    }
};


MessageView ConfirmCreatePromo(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    
    if (service::promo::CreateByDraft(user_id))
    {
        text << "Промокод успешно создан!";
    }
    else 
    {
        text << "Что-то пошло не так";
    }

    keyboard->inlineKeyboard.push_back({MakeButton("Меню", "start")});

    return {
        text.str(),
        keyboard
    };
}


class ConfirmCreatePromoCallback : public Callback {
public:
    std::string name() const override {
        return "confirm_create_promo";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Confirm create promo callback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = ConfirmCreatePromo(query->from->id);

            bot.getApi().editMessageText(
                view.text,
                query->message->chat->id,
                query->message->messageId,
                "",
                "HTML",
                nullptr,
                view.keyboard,
                {}
            );
        } 
        catch (...) 
        {
            Log("Кнопка устарела");
            bot.getApi().sendMessage (
                query->message->chat->id,
                "Кнопка устарела. Используйте /create_promo"
            );
        }
    }
};


std::unique_ptr<Command> createCreatePromoCommand() {
    return std::make_unique<CreatePromoCommand>();
}

std::unique_ptr<Callback> createCreatePromoCallback() {
    return std::make_unique<CreatePromoCallback>();
}

std::unique_ptr<Callback> createCreatePromoBonusCallback() {
    return std::make_unique<CreatePromoBonusCallback>();
}

std::unique_ptr<Callback> createCreatePromoPromoCallback() {
    return std::make_unique<CreatePromoPromoCallback>();
}

std::unique_ptr<Callback> createConfirmCreatePromoCallback() {
    return std::make_unique<ConfirmCreatePromoCallback>();
}


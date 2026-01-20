
#include "bot/commands.hpp"
#include "services/users.hpp"


MessageView Promo (int64_t user_id)
{
    service::users::SetState(user_id, UserState::WaitPromo);
    
    std::ostringstream text;
    text
        << "🎁 Введите промокод:";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "buy_vpn")});

    return {
        text.str(),
        keyboard
    };
}


class PromoCommand : public Command {
public:
    std::string name() const override {
        return "/promo";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Promo command");
        Log(msg);
        
        auto view = Promo(msg->from->id);
        
        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard,
            "HTML"
        );
    }
};


class PromoCallback : public Callback {
public:
    std::string name() const override {
        return "promo";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Promo callback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = Promo(query->from->id);

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
                "Кнопка устарела. Используйте /promo"
            );
        }
    }
};


std::unique_ptr<Command> createPromoCommand() {
    return std::make_unique<PromoCommand>();
}

std::unique_ptr<Callback> createPromoCallback() {
    return std::make_unique<PromoCallback>();
}


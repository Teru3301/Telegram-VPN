
#include "bot/commands.hpp"
#include <sstream>
#include "services/users.hpp"
#include "bot/helper.hpp"


MessageView Buy(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::ostringstream text;
    text << "Приобрести подписку на VPN";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("1 месяц", "plug")});
    keyboard->inlineKeyboard.push_back({MakeButton("3 месяца", "plug")});
    keyboard->inlineKeyboard.push_back({MakeButton("6 месяцев", "plug")});
    keyboard->inlineKeyboard.push_back({MakeButton("🎁 Ввести промокод", "promo")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Назад", "start")});

    return {
        text.str(),
        keyboard
    };
}


class BuyCommand : public Command {
public:
    std::string name() const override {
        return "/buy_vpn";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Buy command");
        auto view = Buy(msg->from->id);
        bot::helper::SendMessage(bot, msg, view);
    }
};


class BuyCallback : public Callback {
public:
    std::string name() const override {
        return "buy_vpn";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        Log("[" + std::to_string(query->from->id) + "] Buy callback");
        auto view = Buy(query->from->id);
        bot::helper::EditMessage(bot, query, view);
    }
};

 
std::unique_ptr<Command> createBuyCommand() {
    return std::make_unique<BuyCommand>();
}

std::unique_ptr<Callback> createBuyCallback() {
    return std::make_unique<BuyCallback>();
}


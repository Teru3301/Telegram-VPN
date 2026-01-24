
#include "bot/commands.hpp"
#include "services/users.hpp"
#include "bot/helper.hpp"


MessageView Help (int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::ostringstream text;
    text
        << "Тут должна быть справка по командам бота\n"
        << "/start - начать пользоваться ботом\n"
        << "/profile - посмотреть свой профиль\n"
        << "/help - показать справку\n"
        << "/howtouse - инструкция для включения VPN\n"
        << "/buy_vpn - получить ключ VPN\n"
        << "/promo - ввести промокод";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Назад", "start")});

    return {
        text.str(),
        keyboard
    };
}


class HelpCommand : public Command {
public:
    std::string name() const override {
        return "/help";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Help command");
        Log(msg);
        auto view = Help(msg->from->id);
        bot::helper::SendMessage(bot, msg, view, "HTML");
    }
};


class HelpCallback : public Callback {
public:
    std::string name() const override {
        return "help";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        Log("[" + std::to_string(query->from->id) + "] Help callback");
        auto view = Help(query->from->id);
        bot::helper::EditMessage(bot, query, view, "HTML");
    }
};


std::unique_ptr<Command> createHelpCommand() {
    return std::make_unique<HelpCommand>();
}

std::unique_ptr<Callback> createHelpCallback() {
    return std::make_unique<HelpCallback>();
}


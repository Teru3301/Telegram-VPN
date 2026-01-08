
#include <tgbot/tgbot.h>
#include "bot/commands.hpp"


class StartCommand : public Command {
public:
    std::string name() const override {
        return "/start";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] StartCommand");
        Log(msg);

        // Клавиатура
        auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();

        TgBot::InlineKeyboardButton::Ptr profileBtn(new TgBot::InlineKeyboardButton);
        profileBtn->text = "👤 Профиль";
        profileBtn->callbackData = "profile";

        TgBot::InlineKeyboardButton::Ptr howToBtn(new TgBot::InlineKeyboardButton);
        howToBtn->text = "📖 Как пользоваться";
        howToBtn->callbackData = "howtouse";

        TgBot::InlineKeyboardButton::Ptr helpBtn(new TgBot::InlineKeyboardButton);
        helpBtn->text = "❓ Помощь";
        helpBtn->callbackData = "help";

        keyboard->inlineKeyboard = {
            { profileBtn },
            { howToBtn },
            { helpBtn }
        };


        bot.getApi().sendMessage(
            msg->chat->id,
            "Добро пожаловать в VPN бот 👋\nВыберите действие:",
            nullptr, nullptr, keyboard, "Markdown"
        );

        bool reg_ok = ReristerUser(msg->from->id, msg->from->username);
        Log(reg_ok ? "A new user has registered" : "The user was not registered");
    }
};


std::unique_ptr<Command> createStartCommand() {
    return std::make_unique<StartCommand>();
}


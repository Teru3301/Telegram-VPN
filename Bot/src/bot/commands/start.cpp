
#include <tgbot/tgbot.h>
#include "bot/commands.hpp"


MessageView Start(int64_t user_id)
{
    std::ostringstream text;
    text << "Добро пожаловать в VPN бот 👋\nВыберите действие:";

    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;    
     
    keyboard->inlineKeyboard.push_back(std::vector<TgBot::InlineKeyboardButton::Ptr>{MakeButton("🔐 Получить ключи VPN", "buy_vpn")});
    keyboard->inlineKeyboard.push_back(std::vector<TgBot::InlineKeyboardButton::Ptr>{MakeButton("📖 Как пользоваться", "howtouse")});
    row.push_back(MakeButton("❓ Команды", "help"));
    row.push_back(MakeButton("👤 Профиль", "profile"));

    keyboard->inlineKeyboard.push_back(row);

    return {
        text.str(),
        keyboard
    };
}


class StartCommand : public Command {
public:
    std::string name() const override {
        return "/start";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] StartCommand");
        Log(msg);
        
        auto view = Start(msg->from->id);

        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard
        );

        bool reg_ok = ReristerUser(msg->from->id, msg->from->username);
        Log(reg_ok ? "A new user has registered" : "The user was not registered");
    }
};


class StartCallback : public Callback {
public:
    std::string name() const override {
        return "start";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] StartCallback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = Start(query->from->id);

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
                "Кнопка устарела. Используйте /start"
            );
        }


        bool reg_ok = ReristerUser(query->from->id, query->from->username);
        Log(reg_ok ? "A new user has registered" : "The user was not registered");
    }
};


std::unique_ptr<Command> createStartCommand() {
    return std::make_unique<StartCommand>();
}

std::unique_ptr<Callback> createStartCallback() {
    return std::make_unique<StartCallback>();
}


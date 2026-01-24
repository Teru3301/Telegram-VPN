
#include <tgbot/tgbot.h>
#include "bot/commands.hpp"
#include "services/users.hpp"
#include "bot/helper.hpp"


MessageView Start(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::ostringstream text;
    text 
        << "🚀 Добро пожаловать в VPN Бот!\n\n"
        << "Здесь ты можешь получить быстрый и надежный VPN доступ всего в пару кликов!\n\n"
        << "🔸 Моментальное подключение\n"
        << "🔸 Защищенное соединение\n"
        << "🔸 Установка за 2 минуты\n"
        << "🔸 Поддерживаемые платформы\n"
        << " - 📱 Android\n - 🖥️ Windows\n - 🐧 Linux\n\n"
        << "👇 Выбери нужный раздел ниже:";

    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;    
     
    keyboard->inlineKeyboard.push_back(std::vector<TgBot::InlineKeyboardButton::Ptr>{MakeButton("🔐 Получить ключи VPN", "buy_vpn")});
    keyboard->inlineKeyboard.push_back(std::vector<TgBot::InlineKeyboardButton::Ptr>{MakeButton("📖 Как пользоваться", "howtouse")});
    row.push_back(MakeButton("❓ Команды", "help"));
    row.push_back(MakeButton("👤 Профиль", "profile"));

    keyboard->inlineKeyboard.push_back(row);

    if (service::users::IsAdmin(user_id))
    {
        text 
        << "\n\nВы назначены администратором этого бота";
        keyboard->inlineKeyboard.push_back(std::vector<TgBot::InlineKeyboardButton::Ptr>{MakeButton("🎁 Создать промокод", "create_promo")});
    }

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
        bool reg_ok = service::users::RegisterNew(msg->from->id, msg->from->username);
        Log(reg_ok ? "A new user has registered" : "The user was not registered");
        if (service::users::IsAdmin(msg->from->username)) service::users::SetAdmin(msg->from->id);
        auto view = Start(msg->from->id);
        bot::helper::SendMessage(bot, msg, view);
    }
};


class StartCallback : public Callback {
public:
    std::string name() const override {
        return "start";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        Log("[" + std::to_string(query->from->id) + "] StartCallback");
        auto view = Start(query->from->id);
        bot::helper::EditMessage(bot, query, view, "HTML");
        bool reg_ok = service::users::RegisterNew(query->from->id, query->from->username);
        Log(reg_ok ? "A new user has registered" : "The user was not registered");
    }
};


std::unique_ptr<Command> createStartCommand() {
    return std::make_unique<StartCommand>();
}

std::unique_ptr<Callback> createStartCallback() {
    return std::make_unique<StartCallback>();
}


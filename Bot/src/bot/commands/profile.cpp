
#include "bot/commands.hpp"
#include <sstream>
#include <iomanip>


MessageView Profile(int64_t user_id)
{
    SetState(user_id, UserState::Idle);
    
    std::ostringstream balance_stream;
    balance_stream << std::fixed << std::setprecision(2)
                   << CheckBalance(user_id);

    struct Key 
    {
        std::string key = "";
        double d_gb = 0.0;
        double u_gb = 0.0;
        std::string end_date = "";
        bool active = false;
    };

    std::vector<Key> keys;
    //Key k1 {"123123123", 12.6, 0.7, "2031.01.01", true};
    //Key k2 {"78yb83bd3", 4.2, 0.2, "2026.05.08", false};
    //keys.push_back(k1);
    //keys.push_back(k2);


    std::ostringstream text;
    text
        << "👤 Ваш профиль\n\n"
        << "Активных ключей: <code>" << keys.size() << "</code>\n\n";

    for (auto key : keys)
    {
        text 
        << "<code>" << key.key << "</code>\n"
        << "Дата окончания: " << key.end_date << "\n"
        << "⬇️ Скачано: " << key.d_gb << "GB\n"
        << "⬆️ Отправлено: " << key.u_gb << "GB\n\n";
    }

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("🔐 Получить ключи VPN", "buy_vpn")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Назад", "start")});

    return {
        text.str(),
        keyboard
    };
}


class ProfileCommand : public Command {
public:
    std::string name() const override {
        return "/profile";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Profile command");

        auto view = Profile(msg->from->id);

        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard,
            "HTML"
        );
    }
};


class ProfileCallback : public Callback {
public:
    std::string name() const override {
        return "profile";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] Profile callback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = Profile(query->from->id);

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
                "Кнопка устарела. Используйте /profile"
            );
        }
    }
};

 
std::unique_ptr<Command> createProfileCommand() {
    return std::make_unique<ProfileCommand>();
}

std::unique_ptr<Callback> createProfileCallback() {
    return std::make_unique<ProfileCallback>();
}


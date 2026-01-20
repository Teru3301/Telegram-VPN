
#include "bot/commands.hpp"
#include <sstream>
#include <iomanip>
#include "services/users.hpp"
#include "services/keys.hpp"


MessageView Profile(int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::vector<Key> keys = service::keys::FindAll(user_id);
    
    std::ostringstream text;
    text
        << "👤 Ваш профиль\n\n"
        << "Активных ключей: <code>" << keys.size() << "</code>\n\n";

    for (auto key : keys)
    {
        auto end_time = std::chrono::system_clock::from_time_t(0) + 
                   std::chrono::milliseconds(key.end_date);
        std::time_t t = std::chrono::system_clock::to_time_t(end_time);
        std::tm tm{};
        localtime_r(&t, &tm);
        text 
        << "<code>" << key.vless_uri << "</code>\n"
        << "Дата окончания: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n"
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


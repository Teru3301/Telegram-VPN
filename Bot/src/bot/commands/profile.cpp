
#include "bot/commands.hpp"
#include <sstream>
#include <iomanip>


MessageView Profile(int64_t user_id)
{
    std::ostringstream balance_stream;
    balance_stream << std::fixed << std::setprecision(2)
                   << CheckBalance(user_id);

    bool subscription_active = false;
    std::string subscription_end_date = "—"; // TODO
    double download_gb = 0.0; // TODO
    double upload_gb   = 0.0; // TODO

    std::ostringstream text;
    text
        << "👤 Профиль\n"
        << "──────────────\n"
        << "💰 Баланс: " << balance_stream.str() << " ₽\n"
        << "📦 Подписка: "
        << (subscription_active ? "Активна\n" : "Не активна\n");

    if (subscription_active) {
        text << "⏳ Окончание: " << subscription_end_date << "\n";
    }

    text
        << "⬇️ Скачано: " << std::fixed << std::setprecision(2) << download_gb << " GB\n"
        << "⬆️ Загружено: " << std::fixed << std::setprecision(2) << upload_gb << " GB\n";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    {
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Временная заглушка";
        btn->callbackData = "plug1";
        row.push_back(btn);
    }
    {
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Временная заглушка";
        btn->callbackData = "plug2";
        row.push_back(btn);
    }

    keyboard->inlineKeyboard.push_back(row);

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
            view.keyboard
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

        auto view = Profile(query->from->id);

        bot.getApi().answerCallbackQuery(query->id);

        bot.getApi().editMessageText(
            view.text,
            query->message->chat->id,
            query->message->messageId,
            "",
            "",
            nullptr,
            view.keyboard,
            {}
        );
    }
};

 
std::unique_ptr<Command> createProfileCommand() {
    return std::make_unique<ProfileCommand>();
}

std::unique_ptr<Callback> createProfileCallback() {
    return std::make_unique<ProfileCallback>();
}


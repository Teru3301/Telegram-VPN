
#include "bot/commands.hpp"
#include "services/users.hpp"


MessageView HowToUse (int64_t user_id)
{
    service::users::SetState(user_id, UserState::Idle);
    
    std::ostringstream text;
    text
        << "Тут будут инструкции для запуска VPN\n"
        << "скорее всего всё будет записано в telegraph, а кнопки будут вести на отдельные статьи для каждой ОС";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    keyboard->inlineKeyboard.push_back({MakeButton("🖥️ Windows", "how_to_windows")});
    keyboard->inlineKeyboard.push_back({MakeButton("🐧 Linux", "how_to_linux")});
    keyboard->inlineKeyboard.push_back({MakeButton("📱Android", "how_to_android")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Назад", "start")});

    return {
        text.str(),
        keyboard
    };
}


class HowToUseCommand : public Command {
public:
    std::string name() const override {
        return "/howtouse";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] HowToUseCommand");
        Log(msg);
        
        auto view = HowToUse(msg->from->id);
        
        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard,
            "HTML"
        );
    }
};


class HowToUseCallback : public Callback {
public:
    std::string name() const override {
        return "howtouse";
    }

    void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) override {
        if (!query || !query->from || !query->message)
            return;

        Log("[" + std::to_string(query->from->id) + "] HowToUseCallback");

        try 
        {
            bot.getApi().answerCallbackQuery(query->id);

            auto view = HowToUse(query->from->id);

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
                "Кнопка устарела. Используйте /howtouse"
            );
        }
    }
};


std::unique_ptr<Command> createHowToUseCommand() {
    return std::make_unique<HowToUseCommand>();
}

std::unique_ptr<Callback> createHowToUseCallback() {
    return std::make_unique<HowToUseCallback>();
}


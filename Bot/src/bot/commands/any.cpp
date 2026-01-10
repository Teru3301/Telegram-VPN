
#include "bot/commands.hpp"
#include "mongo/users.hpp"


MessageView OnIdle(TgBot::Message::Ptr msg)
{
    SetState(msg->from->id, UserState::Idle);
    
    std::ostringstream text;
    text
        << "Я не понимаю эту команду 😕\nИспользуй /help что бы ознакомиться с доступными коммандами";
    
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    return {
        text.str(),
        keyboard
    };
}


MessageView OnWaitPromo(TgBot::Message::Ptr msg)
{
    SetState(msg->from->id, UserState::Idle);
    
    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    
    if (!CheckPromo(msg->text))
    {
        text << "Промокод не верный или истёк";
    }
    else 
    {
        if (UsePromo(msg->from->id, msg->text))
        {
            text 
                << "Промокод успешно активирован!\n"
                << "Перейдите в \"Профиль\" для просмотра бонуса";
            keyboard->inlineKeyboard.push_back({MakeButton("👤 Профиль", "profile")});
        }
        else 
        {
            text
                << "Что-то пошло не так. Попробуйте ещё раз чуть позже";
        }
    }

    keyboard->inlineKeyboard.push_back({MakeButton("Ввести промокод ещё раз", "promo")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    return {
        text.str(),
        keyboard
    };
}

MessageView OnError(TgBot::Message::Ptr msg)
{
    SetState(msg->from->id, UserState::Idle);
    
    std::ostringstream text;
    text
        << "Непредвиденная ошибка на нашей стороне!";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    return {
        text.str(),
        keyboard
    };
}


class AnyCommand : public Command {
public:
    std::string name() const override {
        return "*";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        
        UserState state = GetState(msg->from->id);
        Log("[" + std::to_string(msg->from->id) + "] Any message");
        Log(msg);

        MessageView view;
        switch (state)
        {
            case UserState::Idle:
                Log("UserState = Idle");
                view = OnIdle(msg);
                break;
            
            case UserState::WaitPromo:
                Log("UserState = WaitPromo");
                view = OnWaitPromo(msg);
                break;
            
            default:
                Log("Непредвиденный UserState");
                view = OnError(msg);
        }
        bot.getApi().sendMessage(
            msg->chat->id,
            view.text,
            nullptr, nullptr,
            view.keyboard
        );
    }
};


std::unique_ptr<Command> createAnyCommand() {
    return std::make_unique<AnyCommand>();
}




#include "bot/commands.hpp"
#include "services/users.hpp"
#include "services/promo.hpp"
#include "bot/helper.hpp"


MessageView OnIdle(TgBot::Message::Ptr msg)
{
    service::users::SetState(msg->from->id, UserState::Idle);
    
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
    service::users::SetState(msg->from->id, UserState::Idle);
    
    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    
    if (!service::promo::Check(msg->text))
    {
        text << "Промокод не верный или истёк";
    }
    else 
    {
        if (service::promo::Use(msg->from->id, msg->text))
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


MessageView OnEnterAviableUses(TgBot::Message::Ptr msg)
{
    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    uint64_t uses = 0;
    const uint64_t MAX_USES = 1000000; // ограничение на количество использований

    try
    {
        if (msg->text.empty())
            throw std::invalid_argument("empty input");

        size_t pos = 0;
        uses = std::stoull(msg->text, &pos);

        // проверка на лишние символы и допустимый диапазон
        if (pos != msg->text.size() || uses == 0 || uses > MAX_USES)
            throw std::invalid_argument("invalid range");

        // сохраняем значение в черновик
        service::promo::SetDraftUses(msg->from->id, uses);

        text << "Введите промокод (например FREEVPN)";
        service::users::SetState(msg->from->id, UserState::CreatePromoEnterPromocode);
    }
    catch (const std::exception& e)
    {
        text << "❌ Ошибка!\nВведите целое положительное число от 1 до " << MAX_USES;

        keyboard->inlineKeyboard.push_back({
            MakeButton("🔁 Ввести ещё раз", "bonus")
        });

        service::users::SetState(msg->from->id, UserState::Idle);
    }

    keyboard->inlineKeyboard.push_back({
        MakeButton("🔙 Отмена", "start")
    });

    return {
        text.str(),
        keyboard
    };
}


MessageView OnEnterPromocode(TgBot::Message::Ptr msg)
{
    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::string promo = msg->text;
    //  валидация ввода
    if (!service::promo::Check(promo))
    {
        service::promo::SetDraftPromo(msg->from->id, promo);
        text << "Подтвердите создание промокода";
        service::users::SetState(msg->from->id, UserState::Idle);
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        keyboard->inlineKeyboard.push_back({MakeButton("Создать", "confirm_create_promo")});
    }
    else 
    {
        text << "Что-то не так! Возможно такой промокод уже существует. Попробуйте ввести ещё раз";
        service::users::SetState(msg->from->id, UserState::CreatePromoEnterPromocode);
    }

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return {
        text.str(),
        keyboard
    };
}


MessageView OnError(TgBot::Message::Ptr msg)
{
    service::users::SetState(msg->from->id, UserState::Idle);
    
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
        UserState state = service::users::GetState(msg->from->id);
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
           
            case UserState::CreatePromoAviableUses:
                Log("UserState = CreatePromo");
                view =  OnEnterAviableUses(msg); 
                break;

            case UserState::CreatePromoEnterPromocode:
                Log("UserState = CreatePromo");
                view = OnEnterPromocode(msg);
                break;
            
            default:
                Log("Непредвиденный UserState");
                view = OnError(msg);
        }
        bot::helper::SendMessage(bot, msg, view);
    }
};


std::unique_ptr<Command> createAnyCommand() {
    return std::make_unique<AnyCommand>();
}


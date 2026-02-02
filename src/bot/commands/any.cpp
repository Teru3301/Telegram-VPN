
#include "bot/commands.hpp"
#include "services/users.hpp"
#include "services/promo.hpp"
#include "bot/helper.hpp"


MessageView OnIdle(TgBot::Message::Ptr msg)
{
    Log("[bot] [commands] [OnIdle] [TRY]   [" + std::to_string(msg->from->id) +
        "] handle idle message");

    service::users::SetState(msg->from->id, UserState::Idle);

    std::ostringstream text;
    text
        << "Я не понимаю эту команду 😕\n"
        << "Используй /help что бы ознакомиться с доступными коммандами";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    Log("[bot] [commands] [OnIdle] [OK]    [" + std::to_string(msg->from->id) +
        "] response prepared");

    return { text.str(), keyboard };
}


MessageView OnWaitPromo(TgBot::Message::Ptr msg)
{
    Log("[bot] [commands] [OnWaitPromo] [TRY]   [" + std::to_string(msg->from->id) +
        "] handle promo input");

    service::users::SetState(msg->from->id, UserState::Idle);

    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    if (!service::promo::Check(msg->text))
    {
        text << "Промокод не верный или истёк";
        Log("[bot] [commands] [OnWaitPromo] [FAIL]  [" + std::to_string(msg->from->id) +
            "] promo invalid: " + msg->text);
    }
    else 
    {
        if (service::promo::Use(msg->from->id, msg->text))
        {
            text 
                << "Промокод успешно активирован!\n"
                << "Перейдите в \"Профиль\" для просмотра бонуса";
            keyboard->inlineKeyboard.push_back({MakeButton("👤 Профиль", "profile")});
            Log("[bot] [commands] [OnWaitPromo] [OK]    [" + std::to_string(msg->from->id) +
                "] promo applied: " + msg->text);
        }
        else 
        {
            text << "Что-то пошло не так. Попробуйте ещё раз чуть позже";
            Log("[bot] [commands] [OnWaitPromo] [FAIL]  [" + std::to_string(msg->from->id) +
                "] promo use failed: " + msg->text);
        }
    }

    keyboard->inlineKeyboard.push_back({MakeButton("Ввести промокод ещё раз", "promo")});
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    return { text.str(), keyboard };
}


MessageView OnEnterAviableUses(TgBot::Message::Ptr msg)
{
    Log("[bot] [commands] [OnEnterAviableUses] [TRY]   [" + std::to_string(msg->from->id) +
        "] enter available uses");

    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    uint64_t uses = 0;
    const uint64_t MAX_USES = 1000000;

    try
    {
        if (msg->text.empty())
            throw std::invalid_argument("empty input");

        size_t pos = 0;
        uses = std::stoull(msg->text, &pos);

        if (pos != msg->text.size() || uses == 0 || uses > MAX_USES)
            throw std::invalid_argument("invalid range");

        service::promo::SetDraftUses(msg->from->id, uses);
        service::users::SetState(msg->from->id, UserState::CreatePromoEnterPromocode);

        text << "Введите промокод (например FREEVPN)";
        Log("[bot] [commands] [OnEnterAviableUses] [OK]    [" + std::to_string(msg->from->id) +
            "] valid uses entered: " + std::to_string(uses));
    }
    catch (const std::exception& e)
    {
        text << "❌ Ошибка!\nВведите целое положительное число от 1 до " << MAX_USES;
        keyboard->inlineKeyboard.push_back({MakeButton("🔁 Ввести ещё раз", "bonus")});
        service::users::SetState(msg->from->id, UserState::Idle);

        Log("[bot] [commands] [OnEnterAviableUses] [FAIL]  [" + std::to_string(msg->from->id) +
            "] invalid input: '" + msg->text + "', error: " + e.what());
    }

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return { text.str(), keyboard };
}


MessageView OnEnterPromocode(TgBot::Message::Ptr msg)
{
    Log("[bot] [commands] [OnEnterPromocode] [TRY]   [" + std::to_string(msg->from->id) +
        "] enter promocode");

    std::ostringstream text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::string promo = msg->text;

    if (!service::promo::Check(promo))
    {
        service::promo::SetDraftPromo(msg->from->id, promo);
        text << "Подтвердите создание промокода";
        service::users::SetState(msg->from->id, UserState::Idle);
        keyboard->inlineKeyboard.push_back({MakeButton("Создать", "confirm_create_promo")});

        Log("[bot] [commands] [OnEnterPromocode] [OK]    [" + std::to_string(msg->from->id) +
            "] draft promo set: " + promo);
    }
    else 
    {
        text << "Что-то не так! Возможно такой промокод уже существует. Попробуйте ввести ещё раз";
        service::users::SetState(msg->from->id, UserState::CreatePromoEnterPromocode);

        Log("[bot] [commands] [OnEnterPromocode] [FAIL]  [" + std::to_string(msg->from->id) +
            "] promo exists or invalid: " + promo);
    }

    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Отмена", "start")});

    return { text.str(), keyboard };
}


MessageView OnError(TgBot::Message::Ptr msg)
{
    Log("[bot] [commands] [OnError] [TRY]   [" + std::to_string(msg->from->id) +
        "] handle error");

    service::users::SetState(msg->from->id, UserState::Idle);

    std::ostringstream text;
    text << "Непредвиденная ошибка на нашей стороне!";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    keyboard->inlineKeyboard.push_back({MakeButton("🔙 Меню", "start")});

    Log("[bot] [commands] [OnError] [OK]    [" + std::to_string(msg->from->id) +
        "] error response prepared");

    return { text.str(), keyboard };
}


class AnyCommand : public Command {
public:
    std::string name() const override {
        return "*";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        UserState state = service::users::GetState(msg->from->id);
        Log("[" + std::to_string(msg->from->id) + "] Any message");

        MessageView view;
        switch (state)
        {
            case UserState::Idle:
                view = OnIdle(msg);
                break;
            
            case UserState::WaitPromo:
                view = OnWaitPromo(msg);
                break;
           
            case UserState::CreatePromoAviableUses:
                view =  OnEnterAviableUses(msg); 
                break;

            case UserState::CreatePromoEnterPromocode:
                view = OnEnterPromocode(msg);
                break;
            
            default:
                view = OnError(msg);
        }
        bot::helper::SendMessage(bot, msg, view);
    }
};


std::unique_ptr<Command> createAnyCommand() {
    return std::make_unique<AnyCommand>();
}


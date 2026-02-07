
#include "bot/command.hpp"


TgBot::InlineKeyboardButton::Ptr MakeButton(std::string text, std::string callback)
{
    TgBot::InlineKeyboardButton::Ptr button = std::make_shared<TgBot::InlineKeyboardButton>();

    button->text = text;
    button->callbackData = callback;

    return button;
}


TgBot::InlineKeyboardButton::Ptr MakeUrlButton(std::string text, std::string url)
{
    TgBot::InlineKeyboardButton::Ptr button = std::make_shared<TgBot::InlineKeyboardButton>();

    button->text = text;
    button->url = url;

    return button;
}



#pragma once
#include <tgbot/tgbot.h>
#include <string>


class Command 
{
public:
    virtual ~Command()  = default;
    virtual std::string name() const = 0;
    virtual void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) = 0;
};


class Callback {
public:
    virtual ~Callback() = default;
    virtual std::string name() const = 0;
    virtual void execute(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr query) = 0;
};


struct MessageView {
    std::string text;
    TgBot::InlineKeyboardMarkup::Ptr keyboard;
};


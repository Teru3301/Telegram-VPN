
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



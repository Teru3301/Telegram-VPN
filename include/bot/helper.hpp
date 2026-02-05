
#pragma once

#include <tgbot/tgbot.h>
#include "bot/command.hpp"


namespace bot::helper
{


void SendMessage(
        TgBot::Bot& bot, 
        TgBot::Message::Ptr& msg,
        const MessageView& view, 
        const std::string& parse_mode = ""
        );
void EditMessage(
        TgBot::Bot& bot, 
        TgBot::CallbackQuery::Ptr& query, 
        const MessageView& view, 
        const std::string& parse_mode = ""
        );
std::string BytesToHumanReadable(uint64_t bytes);


}



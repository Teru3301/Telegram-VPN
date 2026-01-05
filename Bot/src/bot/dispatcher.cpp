
#include "bot/dispatcher.hpp"


void CommandDispatcher::add(std::unique_ptr<Command> cmd)
{
    commands_[cmd->name()] = std::move(cmd);
}


void CommandDispatcher::setFallback(std::unique_ptr<Command> cmd)
{
    fallback_ = std::move(cmd);
}


void CommandDispatcher::dispatch(TgBot::Bot& bot, TgBot::Message::Ptr msg)
{
    if (!msg || msg->text.empty())
        return;

    auto it = commands_.find(msg->text);

    if (it != commands_.end()) {
        it->second->execute(bot, msg);
    } else if (fallback_) {
        fallback_->execute(bot, msg);
    }
}



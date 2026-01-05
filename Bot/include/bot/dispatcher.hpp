
#pragma once
#include <unordered_map>
#include <memory>
#include "command.hpp"


class CommandDispatcher {
public:
    void add(std::unique_ptr<Command> cmd);
    void setFallback(std::unique_ptr<Command> cmd);
    void dispatch(TgBot::Bot& bot, TgBot::Message::Ptr msg);

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
    std::unique_ptr<Command> fallback_;
};



#include "bot/commands.hpp"
#include <tgbot/tgbot.h>


class StartCommand : public Command {
public:
    std::string name() const override {
        return "/start";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] StartCommand");
        Log(msg);
        bot.getApi().sendMessage(msg->chat->id, "This is VPN telegram bot");
    }
};


std::unique_ptr<Command> createStartCommand() {
    return std::make_unique<StartCommand>();
}



#include "bot/commands.hpp"


class HowToUseCommand : public Command {
public:
    std::string name() const override {
        return "/howtouse";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] HowToUseCommand");
        Log(msg);
        bot.getApi().sendMessage(msg->chat->id, "Тут будет инструкция для запуска VPN");
    }
};


std::unique_ptr<Command> createHowToUseCommand() {
    return std::make_unique<HowToUseCommand>();
}


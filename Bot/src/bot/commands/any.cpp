
#include "bot/commands.hpp"


class AnyCommand : public Command {
public:
    std::string name() const override {
        return "*";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Any message");
        Log(msg);

        bot.getApi().sendMessage(
            msg->chat->id,
            "Я не понимаю эту команду 😕\nИспользуй /help что бы ознакомиться с доступными коммандами"
        );
    }
};


std::unique_ptr<Command> createAnyCommand() {
    return std::make_unique<AnyCommand>();
}




#include "bot/commands.hpp"
#include <tgbot/tgbot.h>


class ProfileCommand : public Command {
public:
    std::string name() const override {
        return "/profile";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Profile command");
        Log(msg);
        bot.getApi().sendMessage(msg->chat->id, "Profile info");
    }
};

 
std::unique_ptr<Command> createProfileCommand() {
    return std::make_unique<ProfileCommand>();
}


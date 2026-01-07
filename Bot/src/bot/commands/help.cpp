
#include "bot/commands.hpp"


class HelpCommand : public Command {
public:
    std::string name() const override {
        return "/help";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Help command");
        Log(msg);
        bot.getApi().sendMessage(msg->chat->id, 
                "Тут должна быть справка по командам бота\n"
                "/start - начать пользоваться ботом\n"
                "/profile - посмотреть свой профиль\n"
                "/help - показать справку\n"
                "/howtouse - инструкция для включения VPN"
                );
    }
};

 
std::unique_ptr<Command> createHelpCommand() {
    return std::make_unique<HelpCommand>();
}


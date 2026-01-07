
#include "bot/commands.hpp"
#include <sstream>
#include <iomanip>


class ProfileCommand : public Command {
public:
    std::string name() const override {
        return "/profile";
    }

    void execute(TgBot::Bot& bot, TgBot::Message::Ptr msg) override {
        Log("[" + std::to_string(msg->from->id) + "] Profile command");
        Log(msg);

        std::ostringstream balance_stream;
        balance_stream << std::fixed << std::setprecision(2)
                       << CheckBalance(msg->from->id);

        bool subscription_active = false;
        std::string subscription_end_date = "—"; // TODO
        double download_gb = 0.0; // TODO
        double upload_gb   = 0.0; // TODO

        std::ostringstream text;
        text
            << "👤 Профиль\n"
            << "──────────────\n"
            << "💰 Баланс: " << balance_stream.str() << " ₽\n"
            << "📦 Подписка: "
            << (subscription_active ? "Активна\n" : "Не активна\n");

        if (subscription_active) {
            text << "⏳ Окончание: " << subscription_end_date << "\n";
        }

        text
            << "⬇️ Скачано: " << std::fixed << std::setprecision(2) << download_gb << " GB\n"
            << "⬆️ Загружено: " << std::fixed << std::setprecision(2) << upload_gb << " GB\n";

        bot.getApi().sendMessage(msg->chat->id, text.str());
    }
};

 
std::unique_ptr<Command> createProfileCommand() {
    return std::make_unique<ProfileCommand>();
}


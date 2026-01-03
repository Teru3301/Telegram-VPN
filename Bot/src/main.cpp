#include <fstream>
#include <string>
#include <stdio.h>
#include <tgbot/tgbot.h>


std::string loadOrRequestToken(const std::string& filepath) {
    std::string token;
    
    // Пробуем загрузить из файла
    std::ifstream in(filepath);
    if (in && std::getline(in, token) && !token.empty())
        return token;
    
    // Запрашиваем у пользователя
    std::cout << "Enter bot token: ";
    std::getline(std::cin, token);
    
    // Сохраняем в файл
    std::ofstream out(filepath);
    if (out) out << token;
    
    return token;
}


int main() {
    std::string path = "token";
    std::string TOKEN = loadOrRequestToken(path);
    TgBot::Bot bot(TOKEN);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}



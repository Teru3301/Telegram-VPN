
#include <string>
#include <fstream>
#include "loger.hpp"


//  Загрузка или сохранение телеграм бот токена
std::string loadOrRequestToken(const std::string& filepath) 
{
    std::string token;
    
    std::ifstream in(filepath);
    if (in && std::getline(in, token) && !token.empty())
        return token;
    
    std::cout << "Enter bot token: ";
    std::getline(std::cin, token);
    
    std::ofstream out(filepath);
    if (out) out << token;
    
    return token;
}


int main()
{
    std::string path = "token";
    std::string TOKEN = loadOrRequestToken(path);
    TgBot::Bot bot{TOKEN};


    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) 
    {
        bot.getApi().sendMessage(message->chat->id, "This is VPN telegram bot");
    });
   
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) 
    {
        Log(message);
        if (StringTools::startsWith(message->text, "/start")) 
        {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });


    try 
    {
        Log("Bot username: " + bot.getApi().getMe()->username);
        TgBot::TgLongPoll longPoll{bot};
        while (true)
        {
            longPoll.start();
        }
    } 
    catch (TgBot::TgException& e) 
    {
        Log("Error:" + std::string(e.what()));
    }
    
    return 0;
}



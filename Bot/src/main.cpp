
#include <string>
#include <fstream>

#include "bot/dispatcher.hpp" 
#include "bot/commands.hpp"


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


    CommandDispatcher dispatcher;
    dispatcher.setFallback(createAnyCommand());
    dispatcher.add(createStartCommand());
    dispatcher.add(createProfileCommand());
    dispatcher.add(createHelpCommand());
    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr msg)
    {
        dispatcher.dispatch(bot, msg);
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



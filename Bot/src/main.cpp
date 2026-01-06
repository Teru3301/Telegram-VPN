
#include <string>

#include "bot/dispatcher.hpp" 
#include "bot/commands.hpp"
#include "mongo/config.hpp"


int main()
{
    std::string path = "token";
    std::string TOKEN = GetTgBotToken();
    if (TOKEN == "")
    {
        std::cout << "Enter bot token: ";
        std::getline(std::cin, TOKEN);
        SetTgBotToken(TOKEN);
    }
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



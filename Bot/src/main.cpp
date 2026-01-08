
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


    CommandDispatcher cmd_dispatcher;
    cmd_dispatcher.setFallback(createAnyCommand());
    cmd_dispatcher.add(createStartCommand());
    cmd_dispatcher.add(createProfileCommand());
    cmd_dispatcher.add(createHelpCommand());
    cmd_dispatcher.add(createHowToUseCommand());
    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr msg)
    {
        cmd_dispatcher.dispatch(bot, msg);
    });


    CallbackDispatcher cal_dispatcher;
    cal_dispatcher.add(createProfileCallback());
    bot.getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
    {
        cal_dispatcher.dispatch(bot, query);
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


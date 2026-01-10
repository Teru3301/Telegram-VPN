
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
    CallbackDispatcher cal_dispatcher;

    cmd_dispatcher.setFallback(createAnyCommand());
    cmd_dispatcher.add(createStartCommand());
    cal_dispatcher.add(createStartCallback());
    cmd_dispatcher.add(createProfileCommand());
    cal_dispatcher.add(createProfileCallback());
    cmd_dispatcher.add(createHelpCommand());
    cal_dispatcher.add(createHelpCallback());
    cmd_dispatcher.add(createHowToUseCommand());
    cal_dispatcher.add(createHowToUseCallback());
    cmd_dispatcher.add(createBuyCommand());
    cal_dispatcher.add(createBuyCallback());
    cmd_dispatcher.add(createPromoCommand());
    cal_dispatcher.add(createPromoCallback());
    
    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr msg) {cmd_dispatcher.dispatch(bot, msg);});
    bot.getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query) {cal_dispatcher.dispatch(bot, query);});

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


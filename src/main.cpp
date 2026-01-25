
#include <string>

#include "config.hpp"
#include "bot/dispatcher.hpp"
#include "bot/commands.hpp"
#include "xui/services.hpp"
#include "loger.hpp"
#include "mongo/mongo.hpp"
#include "bot/config.hpp"


int main()
{
    Log("------------------------------------------");

    config::CheckEnv("MONGO_URI");
    config::CheckEnv("TG_BOT_TOKEN");
    config::CheckEnv("XUI_URL");

    Database::init(config::GetEnv("MONGO_URI"), "test_db");

    if (!xui::Service::GetConnection()) return 1;
    TgBot::Bot bot{config::GetEnv("TG_BOT_TOKEN")};
    bot::config::LoadAdmins("data/admins.json");

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
    cmd_dispatcher.add(createCreatePromoCommand());
    cal_dispatcher.add(createCreatePromoCallback());
    cal_dispatcher.add(createCreatePromoBonusCallback());
    cal_dispatcher.add(createCreatePromoPromoCallback());
    cal_dispatcher.add(createConfirmCreatePromoCallback());
    
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


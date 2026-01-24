
#include "bot/helper.hpp"
#include "loger.hpp"


namespace bot::helper
{


void SendMessage(
    TgBot::Bot& bot,
    TgBot::Message::Ptr& msg,
    const MessageView& view,
    const std::string& parseMode
    ) 
{
    bot.getApi().sendMessage(
        msg->from->id,
        view.text,
        nullptr,
        nullptr,
        view.keyboard,
        parseMode
    );
}

void EditMessage(
    TgBot::Bot& bot,
    TgBot::CallbackQuery::Ptr& query,
    const MessageView& view,
    const std::string& parseMode
    ) 
{
    if (!query || !query->from || !query->message) return;

    try {
        bot.getApi().answerCallbackQuery(query->id);

        bot.getApi().editMessageText(
            view.text,
            query->message->chat->id,
            query->message->messageId,
            "",
            parseMode,
            nullptr,
            view.keyboard,
            {}
        );
    }
    catch (...) {
        Log("Callback устарел");

        bot.getApi().sendMessage(
            query->message->chat->id,
            "Кнопка устарела. Используйте команду ещё раз"
        );
    }
}


}


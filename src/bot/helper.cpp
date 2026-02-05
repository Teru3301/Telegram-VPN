
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


std::string BytesToHumanReadable(uint64_t bytes)
{
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (bytes < KB)
        return std::to_string(bytes) + " B";
    if (bytes < MB)
        oss << bytes / KB << " KB";
    else if (bytes < GB)
        oss << bytes / MB << " MB";
    else
        oss << bytes / GB << " GB";

    return oss.str();
}


}


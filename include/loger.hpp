
#pragma once
#include <string>
#include <tgbot/tgbot.h>


std::string prefix();


class Logger 
{
public:
    Logger(std::string message)
    {
        this->message = message;
        this->user_id = -1;
        this->user_name = "";
        this->date_time = prefix();
        log();
    }
    Logger(TgBot::Message::Ptr message)
    {
        this->message = message->text;
        this->user_id = message->from->id;
        if (!message->from->username.empty()) this->user_name = message->from->username;
        else this->user_name = "";
        this->date_time = prefix();
        log();
    }
private:
    std::string date_time;
    int user_id;
    std::string user_name;
    std::string message;

    std::string log_file_path = "data/logs";

    void log();
    void log_to_cmd(const std::string& log_message);
    void log_to_file(const std::string& log_message);
};


void Log(std::string message);
void Log(TgBot::Message::Ptr message);



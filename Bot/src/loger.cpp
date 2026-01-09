
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include "loger.hpp"


std::string date_time_prifix()
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&time_t_now);
    std::ostringstream oss;
    oss << std::put_time(&local_time, "[%Y-%m-%d %H:%M:%S]");
    return oss.str();
}


void Logger::log ()
{
    std::string log_message = this->date_time + " ";
    if (this->user_id != -1) log_message += ( "[" + std::to_string(this->user_id) );
    if (this->user_name != "") log_message += ( " @" + this->user_name );
    if (this->user_id != -1 || this->user_name != "") log_message += "] ";
    log_message += this->message;

    log_to_cmd(log_message);
    log_to_file(log_message);
}


void Logger::log_to_cmd(const std::string& log_message)
{
    std::clog << log_message << std::endl;
}


void Logger::log_to_file(const std::string& log_message)
{
    std::ofstream file(this->log_file_path, std::ios::app);   
    if (file.is_open()) 
    {
        file << log_message << std::endl;
        if (file.fail()) 
        {
            std::cerr << "Ошибка записи в файл лога: " << this->log_file_path << std::endl;
        }
        
        file.close();
    }
    else 
    {
        std::cerr << "Не удалось открыть файл лога для записи: " << this->log_file_path << std::endl;
    }
}


void Log(std::string message)
{
    Logger l = Logger(message);
}
void Log(TgBot::Message::Ptr message)
{
    Logger l = Logger(message);
}




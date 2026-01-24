
#include "config.hpp" 
#include <iostream>
#include <cstdlib>
#include "services/users.hpp"


std::string GetToken()
{
    const char* env_token = std::getenv("TG_BOT_TOKEN");
    if (env_token && std::string(env_token).length() > 0)
    {
        return std::string(env_token);
    }
    return "";
}


void AddTgAdmin()
{
    while (true)
    {
        std::cout << "Add a new Telegram administrator? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);

        if (answer == "n" || answer == "N")
        {
            std::cout << "Administrator setup finished.\n";
            break;
        }

        if (answer != "y" && answer != "Y")
        {
            std::cout << "Please enter 'y' or 'n'.\n";
            continue;
        }

        std::cout << "Enter Telegram username (without @): ";
        std::string username;
        std::getline(std::cin, username);

        if (username.empty())
        {
            std::cout << "Username cannot be empty.\n";
            continue;
        }

        if (service::users::SetAdmin(username))
        {
            std::cout << "Administrator @" << username << " added successfully.\n";
        }
        else
        {
            std::cout << "Failed to add administrator @" << username
                      << " (maybe already exists).\n";
        }
    }
}


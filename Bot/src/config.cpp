
#include "config.hpp" 
#include <iostream>
#include <mongo/config.hpp>


std::string GetToken()
{
    std::string token = GetTgBotToken();
    if (token == "")
    {
        std::cout << "Enter bot token: ";
        std::getline(std::cin, token);
        SetTgBotToken(token);
    }
    return token;
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

        if (AddAdmin(username))
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


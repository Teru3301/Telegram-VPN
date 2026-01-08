
#pragma once
#include "loger.hpp"
#include <memory>
#include "bot/command.hpp"
#include "mongo/users.hpp"


std::unique_ptr<Command> createStartCommand();
std::unique_ptr<Command> createProfileCommand();
std::unique_ptr<Command> createAnyCommand();
std::unique_ptr<Command> createHelpCommand();
std::unique_ptr<Command> createHowToUseCommand();

std::unique_ptr<Callback> createProfileCallback();


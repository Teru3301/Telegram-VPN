
#pragma once
#include "loger.hpp"
#include <memory>
#include "bot/command.hpp"


std::unique_ptr<Command> createStartCommand();
std::unique_ptr<Command> createProfileCommand();
std::unique_ptr<Command> createAnyCommand();
std::unique_ptr<Command> createHelpCommand();



#pragma once
#include "loger.hpp"
#include <memory>
#include "bot/command.hpp"
#include "mongo/user_calls.hpp"


std::unique_ptr<Command> createStartCommand();
std::unique_ptr<Callback> createStartCallback();

std::unique_ptr<Command> createProfileCommand();
std::unique_ptr<Callback> createProfileCallback();

std::unique_ptr<Command> createAnyCommand();

std::unique_ptr<Command> createHelpCommand();
std::unique_ptr<Callback> createHelpCallback();

std::unique_ptr<Command> createHowToUseCommand();
std::unique_ptr<Callback> createHowToUseCallback();

std::unique_ptr<Command> createBuyCommand();
std::unique_ptr<Callback> createBuyCallback();

std::unique_ptr<Command> createPromoCommand();
std::unique_ptr<Callback> createPromoCallback();

std::unique_ptr<Command> createCreatePromoCommand();
std::unique_ptr<Callback> createCreatePromoCallback();
std::unique_ptr<Callback> createCreatePromoBonusCallback();
std::unique_ptr<Callback> createCreatePromoPromoCallback();
std::unique_ptr<Callback> createConfirmCreatePromoCallback();


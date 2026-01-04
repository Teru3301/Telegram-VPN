cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(bot)


include_directories(
    /usr/include/tgbot
    include
    lib
)


add_executable(bot
    ./src/loger.cpp
    ./src/main.cpp
)


target_link_libraries(bot
    TgBot
    pthread
    ssl
    crypto
    curl
    z
)



cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(bot)


find_package(PkgConfig REQUIRED)

pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
pkg_check_modules(BSONCXX REQUIRED libbsoncxx)


include_directories(
    /usr/include/tgbot
    include
    lib
    ${MONGOCXX_INCLUDE_DIRS}
    ${BSONCXX_INCLUDE_DIRS}
)


add_executable(bot
    ./src/loger.cpp
    ./src/main.cpp
    ./src/bot/dispatcher.cpp 
    ./src/bot/commands/start.cpp
    ./src/bot/commands/profile.cpp
    ./src/bot/commands/any.cpp
    ./src/bot/commands/help.cpp
    ./src/mongo/mongo.cpp 
    ./src/mongo/config.cpp
    ./src/mongo/users.cpp 
    ./src/bot/commands/howtouse.cpp
    ./src/bot/commands/buy.cpp
    ./src/bot/command.cpp
)


link_directories(
    ${MONGOCXX_LIBRARY_DIRS}
    ${BSONCXX_LIBRARY_DIRS}
)


target_link_libraries(bot
    TgBot
    pthread
    ssl
    crypto
    curl
    z
    ${MONGOCXX_LIBRARIES}
    ${BSONCXX_LIBRARIES}
)



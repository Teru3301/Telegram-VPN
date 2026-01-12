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


file(GLOB_RECURSE BOT_SRC "src/*.cpp")
add_executable(bot ${BOT_SRC})


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



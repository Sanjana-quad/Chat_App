cmake_minimum_required(VERSION 3.14)
project(CPPChatApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(chat_server server.cpp)
add_executable(chat_client client.cpp)

find_package(Threads REQUIRED)
target_link_libraries(chat_server Threads::Threads)
target_link_libraries(chat_client Threads::Threads)

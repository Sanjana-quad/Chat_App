cmake_minimum_required(VERSION 3.14)
project(CPPChatApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(chat_server server.cpp)
add_executable(chat_client client.cpp)

find_package(Threads REQUIRED)
target_link_libraries(chat_server Threads::Threads)
target_link_libraries(chat_client Threads::Threads)

# Set path to the googletest directory
add_subdirectory(C:\Users\Sanjana\chat_app\build)

enable_testing()

add_executable(tests test_utils.cpp)
target_link_libraries(tests gtest gtest_main pthread)

add_test(NAME all_tests COMMAND tests)

add_subdirectory(tests)

cmake_minimum_required(VERSION 3.10)
project(ChatApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server.cpp)
add_executable(client client.cpp)

# Add googletest subdirectory (change path if you put googletest elsewhere)
add_subdirectory(googletest)

# Enable testing and add tests subdirectory
enable_testing()
add_subdirectory(tests)

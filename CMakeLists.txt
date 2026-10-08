cmake_minimum_required(VERSION 3.20)
project(feed_handler CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(feed_core
    src/order_book.cpp
    src/feed_handler.cpp
)
target_include_directories(feed_core PUBLIC include)

add_executable(generate_feed src/generate_feed.cpp)
target_link_libraries(generate_feed PRIVATE feed_core)

add_executable(feed_handler_main src/main.cpp)
target_link_libraries(feed_handler_main PRIVATE feed_core)

add_executable(book_tests tests/book_tests.cpp)
target_link_libraries(book_tests PRIVATE feed_core)

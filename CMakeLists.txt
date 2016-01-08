cmake_minimum_required(VERSION 3.3)
project(podbot)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic")
#set_property(TARGET podbot PROPERTY CXX_STANDARD 14)
#set macro directory
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)


find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED log thread atomic system filesystem chrono program_options date_time regex)
find_package(Gumbo REQUIRED)
find_package(MySQL REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
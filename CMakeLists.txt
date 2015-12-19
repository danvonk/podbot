cmake_minimum_required(VERSION 2.8.9)
project(podbot)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpermissive")
#set macro directory
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS asio filesystem REQUIRED)
#find_package(Gumbo REQUIRED)

add_subdirectory(src)
# root / cmake script
cmake_minimum_required(VERSION 3.3)
project(pb)

if (LINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic -fsanitize=address")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic")
endif(LINUX)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif(WIN32)

#set_property(TARGET podbot PROPERTY CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
#set macro directory
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED log thread atomic system filesystem chrono program_options date_time regex)
find_package(Gumbo REQUIRED)
find_package(MySQL REQUIRED)

set( gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" )
set(gtest_force_shared_crt ON)
add_subdirectory(dep/googletest)
add_subdirectory(src)


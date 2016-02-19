# root/ cmake script
cmake_minimum_required(VERSION 3.3)
project(pb)

if (LINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic -fsanitize=address")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic")
	set_property(CXX_STANDARD 14)
endif(LINUX)

if (WIN32)
	if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1)
		add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7
	elseif(${CMAKE_SYSTEM_VERSION} EQUAL 10.0)
		add_definitions(-D_WIN32_WINNT=0x0A00) # Windows 10
	endif()

	if (MSVC)
	    # hack to get around the Debug and Release directories cmake tries to add on Windows
	    
	    #set_target_properties (runpodbot PROPERTIES PREFIX "../")
	    
	    SET (LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
	    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
	    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
	    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
	    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
	    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
	    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
	endif(MSVC)

endif(WIN32)

#set macro directory
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED log thread atomic system filesystem chrono program_options date_time regex)
find_package(Gumbo REQUIRED)
find_package(MySQL REQUIRED)

set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" )
set(gtest_force_shared_crt ON)
add_subdirectory(dep/googletest)
add_subdirectory(src)


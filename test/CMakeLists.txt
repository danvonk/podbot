#test/ folder

add_subdirectory(dep/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(podtest
        main.cpp)
target_link_libraries(podtest gtest gtest_main)
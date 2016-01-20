# src/ directory script

add_subdirectory(podbot)
add_subdirectory(test)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/podbot
        ${Boost_INCLUDE_DIRS}
        ${MYSQL_INCLUDE_DIR}
)
add_executable(runpodbot ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(runpodbot podbot ${Boost_LIBRARIES} ${MYSQL_LIBRARIES})
install(TARGETS runpodbot RUNTIME DESTINATION bin)

if(WIN32)
    target_compile_definitions(runpodbot PRIVATE _WIN32_WINNT=0x0601)
    set_target_properties(runpodbot PROPERTIES LIBRARY_OUTPUT_DIRECTORY bin)
    set_target_properties(runpodbot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG binn)
    set_target_properties(runpodbot PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE bin)
endif(WIN32)

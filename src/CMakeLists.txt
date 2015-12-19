# src/ folder
file(GLOB_RECURSE sources_main *h *.hpp *.cpp *.c)
file(GLOB_RECURSE sources_url url/*.h url/*.cc)

file(GLOB_RECURSE pb_root *.h *.hpp *.cpp *.c *.cc *.inl)
file(GLOB_RECURSE pb_http http/*.h http/*.hpp http/*.cpp http/*.c http/*.cc http/*.inl)
file(GLOB_RECURSE pb_common common/*.h common/*.hpp common/*.cpp common/*.c common/*.cc common/*.inl)


source_group(http FILES ${pb_http})
source_group(common FILES ${pb_common})

set(sources_all
        ${pb_http}
        ${pb_common}
        ${pb_root}
)

include_directories(
        podbot
        ${OPENSSL_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIRS}
        #${Gumbo_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(podbot ${sources_all})
target_link_libraries(podbot
        ${BOOST_LIBRARIES}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${OPENSSL_SSL_LIBRARY}
)
install(TARGETS podbot RUNTIME DESTINATION bin)

if(WIN32)
        target_compile_definitions(podbot PRIVATE _WIN32_WINNT=0x0601)
endif(WIN32)
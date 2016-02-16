# test/ directory script

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(
        ${MYSQL_INCLUDE_DIR}
        ${Gumbo_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src/podbot
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)


file(GLOB_RECURSE pb_tests *.h *.hpp *.cpp *.c *.cc *.inl)

add_executable(podtest
        ${pb_tests})

# target_link_libraries(podtest
#         ${Gumbo_LIBRARIES}
#         ${MYSQL_LIBRARIES}
#         ${Boost_LIBRARIES}
#         ${OPENSSL_CRYPTO_LIBRARY}
#         ${OPENSSL_SSL_LIBRARY}
# )
target_link_libraries(podtest gtest gtest_main)
target_link_libraries(podtest podbot)
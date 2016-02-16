# src/ directory script

add_subdirectory(podbot)
add_subdirectory(test)

include_directories(
	${CMAKE_SOURCE_DIR}/src/podbot
	${Boost_INCLUDE_DIRS}
	${MYSQL_INCLUDE_DIR}
	${OPENSSL_INCLUDE_DIR}
	)
add_executable(runpodbot ${CMAKE_SOURCE_DIR}/src/main.cpp)
#target_link_libraries(runpodbot podbot ${Boost_LIBRARIES} ${MYSQL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
target_link_libraries(runpodbot podbot)
install(TARGETS runpodbot RUNTIME DESTINATION bin)

if (MSVC)
    # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows
    set_target_properties (runpodbot PROPERTIES PREFIX "../")
    #SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    #SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
    #SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    #SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
    #SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    #SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
endif(MSVC)

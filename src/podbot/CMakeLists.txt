# src/podbot/ directory script
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE pb_root *.h *.hpp *.cpp *.c *.cc *.inl)
file(GLOB_RECURSE pb_http http/*.h http/*.hpp http/*.cpp http/*.c http/*.cc http/*.inl)
file(GLOB_RECURSE pb_common common/*.h common/*.hpp common/*.cpp common/*.c common/*.cc common/*.inl)
file(GLOB_RECURSE pb_database db/*.h db/*.hpp db/*.cpp db/*.c db/*.cc db/*.inl)
file(GLOB_RECURSE pb_models models/*.h models/*.hpp models/*.cpp models/*.c models/*.cc models/*.inl)

include_directories(
        ${MYSQL_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Gumbo_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src/podbot
        )

set(pb_all
        ${pb_models}
        ${pb_database}
        ${pb_http}
        ${pb_common}
        ${pb_root}
        )

source_group(http FILES ${pb_http})
source_group(common FILES ${pb_common})
source_group(db FILES ${pb_database})
source_group(models FILES ${pb_models})

add_library(podbot STATIC ${pb_all})
target_link_libraries(podbot
        ${Gumbo_LIBRARIES}
        ${MYSQL_LIBRARIES}
        ${Boost_LIBRARIES}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${OPENSSL_SSL_LIBRARY}
        )

if (LINUX)
       target_link_libraries(podbot pthread)
endif(LINUX)

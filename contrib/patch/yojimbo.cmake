cmake_minimum_required(VERSION 3.15)

# https://github.com/sweetdream165/yojimbo-cmake/blob/510da499965f71b741f1155c6e1d8901bc8f3ad9/CMakeLists.txt
# https://github.com/mas-bandwidth/yojimbo/issues/133

project(yojimbo VERSION 1.2.5)
# Includedirs  
include_directories(
    .
    include
    serialize
    sodium
    netcode
    reliable
    tlsf
)
# Definitions
add_compile_definitions(
    # Debug
    $<$<CONFIG:Debug>:YOJIMBO_DEBUG>
    $<$<CONFIG:Debug>:NETCODE_DEBUG>
    $<$<CONFIG:Debug>:RELIABLE_DEBUG>
    # Release
    $<$<CONFIG:Release>:YOJIMBO_RELEASE>
    $<$<CONFIG:Release>:NETCODE_RELEASE>
    $<$<CONFIG:Release>:RELIABLE_RELEASE>
)
# Out directories
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/run)

# SODIUM
file(GLOB sodium_SRC
    IF(WIN32)
        "sodium/*.c"
        "sodium/*.h"
    ELSE()
        "sodium/*.S"
    ENDIF()
)
file(GLOB sodium_H
    "sodium/*.h"
)
add_library(sodium STATIC ${sodium_SRC})
# set_target_properties(sodium PROPERTIES PUBLIC_HEADER "${sodium_H}")

# NETCODE
add_library(netcode STATIC 
    netcode/netcode.c
    netcode/netcode.h
)
target_compile_definitions(netcode PRIVATE NETCODE_ENABLE_TESTS=1)

# RELIABLE
add_library(reliable STATIC 
    reliable/reliable.c
    reliable/reliable.h
)
target_compile_definitions(reliable PRIVATE RELIABLE_ENABLE_TESTS=1=1)

# TLSF
add_library(tlsf STATIC 
    tlsf/tlsf.c
    tlsf/tlsf.h
)

# YOJIMBO
file(GLOB yojimbo_SRC
    "include/*.h"
    "source/*.cpp"
)
file(GLOB yojimbo_H
    "include/*.h"
)
add_library(yojimbo STATIC ${yojimbo_SRC})
target_link_libraries(yojimbo sodium)
set_target_properties(yojimbo PROPERTIES PUBLIC_HEADER "${yojimbo_H}")

if (WIN32 OR UNIX)
# CLIENT
add_executable(client
    client.cpp
    shared.h
)
target_link_libraries(client
    yojimbo
    sodium
    tlsf
    netcode
    reliable
)

# SERVER
add_executable(server
    server.cpp
    shared.h
)
target_link_libraries(server
    yojimbo
    sodium
    tlsf
    netcode
    reliable
)

# LOOPBACK
add_executable(loopback
    loopback.cpp
    shared.h
)
target_link_libraries(loopback
    yojimbo
    sodium
    tlsf
    netcode
    reliable
)

# SOAK
add_executable(soak
    soak.cpp
    shared.h
)
target_link_libraries(soak
    yojimbo
    sodium
    tlsf
    netcode
    reliable
)

install(TARGETS netcode tlsf reliable client server loopback soak)
endif ()

if (0)
# TEST
add_executable(test
    test.cpp
    shared.h
)
target_link_libraries(test
    yojimbo
    sodium
    tlsf
    netcode
    reliable
)
target_compile_definitions(test PRIVATE SERIALIZE_ENABLE_TESTS=1)
endif ()

install(TARGETS yojimbo sodium EXPORT yojimboConfig PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/yojimbo)
install(EXPORT yojimboConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/yojimbo)

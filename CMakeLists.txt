# cmake min version
cmake_minimum_required(VERSION 3.18.0)

# project version
project(Engine VERSION 0.1.0)

# cxx standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# default build type is Release
if (NOT CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BINARY_DIR} CMAKE_BINARY_DIR_LOWER)
    if (CMAKE_BINARY_DIR_LOWER MATCHES "debug$")
        set(CMAKE_BUILD_TYPE "Debug")
    elseif (CMAKE_BINARY_DIR_LOWER MATCHES "relwithdebinfo$")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    elseif (CMAKE_BINARY_DIR_LOWER MATCHES "release$")
        set(CMAKE_BUILD_TYPE "Release")
    else ()
        # default
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    endif ()
endif ()
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# https://issuetracker.google.com/issues/37128614
# Gradle build RelWithDebInfo by default
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
if (ANDROID AND CMAKE_BUILD_TYPE_LOWER MATCHES "relwithdebinfo")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# enabled only Release, RelWithDebInfo, Debug build types
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
set(ALLOWED_BUILD_TYPES_LOWER ${ALLOWED_BUILD_TYPES})
list(TRANSFORM ALLOWED_BUILD_TYPES_LOWER TOLOWER)
if (NOT ${CMAKE_BUILD_TYPE_LOWER} IN_LIST ALLOWED_BUILD_TYPES_LOWER)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not one of ${ALLOWED_BUILD_TYPES}")
endif ()

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
if (CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    string(APPEND CMAKE_MSVC_RUNTIME_LIBRARY "Debug")
endif ()

# cmake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# include modules
include(Platform)
include(GitHash)
include(ProcessorCount)

# set parallel level
processorcount(PROCESSOR_COUNT)
set(CMAKE_BUILD_PARALLEL_LEVEL ${PROCESSOR_COUNT})

# set artifact dir
set(ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/artifacts)
file(MAKE_DIRECTORY ${ARTIFACT_DIR})

# external targets
file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/external)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/external)

execute_process(
        COMMAND ${CMAKE_COMMAND}
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DANDROID_NDK=${ANDROID_NDK}
        -DANDROID_ABI=${ANDROID_ABI}
        -DANDROID_PLATFORM=${ANDROID_PLATFORM}
        ${CMAKE_SOURCE_DIR}/contrib
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external
)

execute_process(
        COMMAND ${CMAKE_COMMAND} --build . -j ${CMAKE_BUILD_PARALLEL_LEVEL}  --config ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external
)

add_subdirectory(contrib)

# engine sources
add_subdirectory(source)

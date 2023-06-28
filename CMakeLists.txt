# cmake min version
cmake_minimum_required(VERSION 3.18.0)


# project version
project(ENGINE VERSION 0.1.0)


# cxx standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# cmake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


# default build type is Release
if (NOT CMAKE_BUILD_TYPE)
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}" BINARY_DIR)
    string(TOLOWER ${BINARY_DIR} BINARY_DIR_LOWER)
    if (BINARY_DIR_LOWER MATCHES "debug$")
        message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to Debug")
        set(CMAKE_BUILD_TYPE "Debug")
    elseif (BINARY_DIR_LOWER MATCHES "relwithdebinfo$")
        message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to RelWithDebInfo")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    elseif (BINARY_DIR_LOWER MATCHES "release$")
        message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to Release")
        set(CMAKE_BUILD_TYPE "Release")
    else ()
        message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to Release")
        set(CMAKE_BUILD_TYPE "Release")
    endif ()
endif ()


# enabled only Release, RelWithDebInfo, Debug build types
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
set(ALLOWED_BUILD_TYPES_LOWER ${ALLOWED_BUILD_TYPES})
list(TRANSFORM ALLOWED_BUILD_TYPES_LOWER TOLOWER)
if (NOT ${CMAKE_BUILD_TYPE_LOWER} IN_LIST ALLOWED_BUILD_TYPES_LOWER)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()


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


# contrib
add_subdirectory(contrib)
list(APPEND CMAKE_PREFIX_PATH ${DEPS_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${DEPS_ROOT})


# engine sources
add_subdirectory(source)


# extra targets for IDEs
add_custom_target(BuildPackage ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package --config ${CMAKE_BUILD_TYPE})
add_custom_target(CleanPackage ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean --config ${CMAKE_BUILD_TYPE})

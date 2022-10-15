# cmake min version
cmake_minimum_required(VERSION 3.18.1)


# project version
project(engine VERSION 0.1.0)


# cxx standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# cmake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


# include modules
include(Platform)


# default build type is Release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()


# enabled only Release, RelWithDebInfo, Debug build types
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
if (NOT ${CMAKE_BUILD_TYPE} IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()


# set artifact dir
set(ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/artifacts)


# contrib
add_subdirectory(contrib)
list(APPEND CMAKE_PREFIX_PATH ${DEPS_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${DEPS_ROOT})


# engine sources
add_subdirectory(source)


# cpack
set(PACKAGE_NAME "GlueSample")
set(ARTIFACT_NAME ${PACKAGE_NAME}_${TOOLCHAIN_SHORT})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
endif ()
if (WIN32)
    install(FILES DESTINATION .)
endif ()
set(CPACK_PACKAGE_DIRECTORY ${ARTIFACT_DIR})
set(CPACK_PACKAGE_FILE_NAME ${ARTIFACT_NAME})
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "TXZ")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_GENERATOR "TXZ")
else ()
    set(CPACK_GENERATOR "TXZ")
endif ()
include(CPack)

# This file is part of Glue Engine. Created by Andrey Vasiliev

cmake_minimum_required(VERSION 3.18.1)
project(GlueEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


# Include necessary submodules
include(Platform)
include(FlatZip)


# Default build type is Release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()


# Enable only Release, RelWithDebInfo, Debug build types
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
if (NOT ${CMAKE_BUILD_TYPE} IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()


# Create required directories
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/artifacts)
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/tmp)


# Dependencies
add_subdirectory(thirdparty)


list(APPEND CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})

# Engine libraries
add_subdirectory(source)


# assets
file(REMOVE_RECURSE ${GLUE_TMP_DIR})
make_directory(${GLUE_TMP_DIR})
CopyFlatZipDirectory(${CMAKE_SOURCE_DIR}/source/Programs ${GLUE_TMP_DIR}/Programs)
if (ANDROID OR IOS OR EMSCRIPTEN)
    file(GLOB files ${CMAKE_SOURCE_DIR}/source/GLSLES/*)
else ()
    file(GLOB files ${CMAKE_SOURCE_DIR}/source/GLSL/*)
endif ()
file(COPY ${files} DESTINATION ${GLUE_TMP_DIR}/Programs)
ZipDirectory(${GLUE_TMP_DIR}/Programs)
FlatZipDirectory(${CMAKE_SOURCE_DIR}/source/Assets ${GLUE_TMP_DIR}/Assets)


# package helper target
set(GLUE_PACKAGE_NAME "GlueSample")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
endif ()
if (WIN32)
    install(FILES DESTINATION .)
endif ()


set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${GLUE_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "GlueSample")
set(CPACK_PACKAGE_VENDOR "Andrey Vasiliev")

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

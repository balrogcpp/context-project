# This file is part of Glue Engine. Created by Andrey Vasiliev

cmake_minimum_required(VERSION 3.18.1)
project(GlueEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# VS project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# CMake module path
set(GLUE_CMAKE_DIR ${PROJECT_SOURCE_DIR}/CMake)
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR})


# Include necessary submodules
include(Platform)
include(CMakeDependentOption)
include(InsertDependency)
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
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/tmp)


# Dependencies
add_subdirectory(Dependencies)


list(APPEND CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    insert_dependency(X11)
endif ()
if (DESKTOP)
    insert_dependency(OpenGL)
endif ()
insert_dependency_static(Bullet)
mark_as_advanced(BULLET_INCLUDE_DIR)
insert_dependency_static2(Lua LUA_LIBRARY)
insert_dependency(sol2)
insert_dependency2(OpenAL OPENAL_FOUND)
insert_dependency_static(Ogg)
insert_dependency_static(Vorbis)
insert_dependency_static(SDL2)
insert_dependency_static(Freetype)
insert_dependency_static(pugixml)
set(OGRE_STATIC 1)
insert_dependency_static(OGRE)


if (ANY_NOT_FOUND)
    message(WARNING "Build cmake target Dependencies to get all dependencies. Then configure cmake again to enable GlueLibrary and GlueExample targets. Exiting")
endif ()


if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Source/Engine/android/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy Dependencies)
    return()
endif ()


# Engine libraries
add_subdirectory(Source)


# Examples
add_subdirectory(Example)


# assets
file(REMOVE_RECURSE ${GLUE_TMP_DIR})
make_directory(${GLUE_TMP_DIR})
CopyFlatZipDirectory(${CMAKE_SOURCE_DIR}/Source/Programs ${GLUE_TMP_DIR}/Programs)
if (ANDROID OR IOS OR EMSCRIPTEN)
    file(GLOB files ${CMAKE_SOURCE_DIR}/Source/GLSLES/*)
else ()
    file(GLOB files ${CMAKE_SOURCE_DIR}/Source/GLSL/*)
endif ()
file(COPY ${files} DESTINATION ${GLUE_TMP_DIR}/Programs)
ZipDirectory(${GLUE_TMP_DIR}/Programs)
FlatZipDirectory(${CMAKE_SOURCE_DIR}/Example/Assets ${GLUE_TMP_DIR}/Assets)


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

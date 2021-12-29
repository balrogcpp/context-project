cmake_minimum_required(VERSION 3.19.8)
project(PrjGlue VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
if (NOT ${CMAKE_BUILD_TYPE} IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()

set(GLUE_CMAKE_DIR ${PROJECT_SOURCE_DIR}/CMake)
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR})
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR}/Package)
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR}/Module)

include(Make)
include(Platform)
include(CppFlags)
include(GitHash)
include(InsertDependency)
include(AppleThreadFix)

# Create artifacts directory
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/Tmp)
make_directory(${GLUE_ARTIFACT_DIR})
make_directory(${GLUE_TMP_DIR})

# Insert dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty)

# Doxygen
include(Doxygen)

set(CMAKE_PREFIX_PATH ${GLUE_EXTERNAL_INSTALL_LOCATION})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_EXTERNAL_INSTALL_LOCATION})

if (NOT MOBILE)
    insert_dependency(OpenGL)
else ()
    insert_dependency(OpenGLES2)
endif ()
insert_dependency(Threads)
insert_dependency_static(OGRE)
insert_dependency_static(Bullet)
insert_dependency_static(ZLIB)
insert_dependency(Lua51)
insert_dependency(OpenAL)
insert_dependency_static(Ogg)
insert_dependency_static(SDL2)
insert_dependency_static(PNG)
insert_dependency_static(Freetype)
insert_dependency_static(assimp)
find_library(ASSIMP_IRRXML_LIBRARY IrrXML QUIET)

if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Android/app/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy ThirdParty)
endif ()

set(CMAKE_FOLDER Package)

# Docker-compose targets (not crossplatform )
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
add_custom_target(DockerComposeDep
        COMMAND docker-compose -f docker-compose-3rd.yml up
        COMMAND docker-compose -f docker-compose-3rd.yml down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building thirdparty libraries with docker-compose..."
        USES_TERMINAL
        )

add_custom_target(DockerCompose
        COMMAND GIT_HASH=${GIT_SHA1} docker-compose up
        COMMAND docker-compose down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building tests with docker..."
        USES_TERMINAL
        )
endif ()

if (ANY_NOT_FOUND)
    return()
endif ()

add_subdirectory(${CMAKE_SOURCE_DIR}/Source)


#Packaging
include(Packaging)

# Doesn't work with MSVC
if (NOT MSVC)
    add_custom_target(BuildPackage
            DEPENDS AssetsBuild
            COMMAND ${CMAKE_COMMAND} --build . --target package
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Build packages..."
            USES_TERMINAL
            )
endif ()

if (WIN32)
    add_custom_target(BuildAndroid
            DEPENDS AssetsBuild
            COMMAND gradlew.bat assembleRelease
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Android
            COMMENT "Build packages..."
            USES_TERMINAL
            )
elseif (UNIX)
    add_custom_target(BuildAndroid
            DEPENDS AssetsBuild
            COMMAND ./gradlew assembleRelease
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Android
            COMMENT "Build packages..."
            USES_TERMINAL
            )
endif ()

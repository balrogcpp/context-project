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
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR}/Package)
list(APPEND CMAKE_MODULE_PATH ${GLUE_CMAKE_DIR}/Module)


# Include necessary submodules
include(Make)
include(Platform)
include(GitHash)
include(CMakeDependentOption)
include(InsertDependency)
include(Doxygen)


# Default build type is RelWithDebInfo
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, CMAKE_BUILD_TYPE defaulted to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()


# Enable only Release, RelWithDebInfo, Debug build types
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
if (NOT ${CMAKE_BUILD_TYPE} IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()


# Create required directories
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/Tmp)
make_directory(${GLUE_ARTIFACT_DIR})
make_directory(${GLUE_TMP_DIR})


# Docker helper target
set(CMAKE_FOLDER Docker) # VS folder

add_custom_target(DockerComposeDep
        COMMAND docker-compose -f docker-compose-3rd.yml up
        COMMAND docker-compose -f docker-compose-3rd.yml down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building thirdparty libraries with docker-compose..."
        USES_TERMINAL
        )

add_custom_target(DockerCompose
        COMMAND ${CMAKE_COMMAND} -E env GIT_HASH=${GIT_SHA1} docker-compose up
        COMMAND docker-compose down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building tests with docker-compose..."
        USES_TERMINAL
        )

set(CMAKE_FOLDER "")


# Android helper targets
# env variable ANDROID_SDK_ROOT must be set manually
set(GLUE_ANDROID_ROOT ${CMAKE_SOURCE_DIR}/Engine/Android)

if (NOT RELEASE)
    set(GRADLE_TARGET assembleDebug)
    set(APK_FILE ${GLUE_ANDROID_ROOT}/app/build/outputs/apk/debug/app-arm64-v8a-debug.apk)
    set(OUTPUT_FILE ${GLUE_PACKAGE_NAME}_Android_aarch64_Debug_${GIT_SHA1}.apk)
else ()
    set(GRADLE_TARGET assembleRelease)
    set(APK_FILE ${GLUE_ANDROID_ROOT}/app/build/outputs/apk/release/app-arm64-v8a-release.apk)
    set(OUTPUT_FILE ${GLUE_PACKAGE_NAME}_Android_aarch64_${GIT_SHA1}.apk)
endif ()


set(CMAKE_FOLDER Android) # VS folder

add_custom_target(BuildAndroid
        DEPENDS BuildAssets
        COMMAND ./gradlew ${GRADLE_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy ${APK_FILE} ${GLUE_ARTIFACT_DIR}/${OUTPUT_FILE}
        WORKING_DIRECTORY ${GLUE_ANDROID_ROOT}
        COMMENT "Building Android package..."
        USES_TERMINAL
        )

add_custom_target(ClearAndroid
        COMMAND ${CMAKE_COMMAND} -E rm -rf .gradle app/.cxx app/build
        WORKING_DIRECTORY ${GLUE_ANDROID_ROOT}
        COMMENT "Deleting Android build files..."
        USES_TERMINAL
        )

set(CMAKE_FOLDER "")


# Dependencies
add_subdirectory(Engine/Dependencies)
set(CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})

if (NOT MOBILE)
    insert_dependency(OpenGL)
else ()
    insert_dependency(OpenGLES2)
endif ()
insert_dependency(Threads)
insert_dependency_static(Bullet)
mark_as_advanced(BULLET_INCLUDE_DIR)
insert_dependency_static(ZLIB)
insert_dependency(Lua51)
insert_dependency(OpenAL)
insert_dependency_static(Ogg)
insert_dependency_static(Vorbis)
insert_dependency_static(SDL2)
insert_dependency_static(PNG)
insert_dependency_static(Freetype)
insert_dependency_static(pugixml)
set(OGRE_STATIC 1)
insert_dependency_static(OGRE)

if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Engine/Android/app/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy Dependencies)
endif ()

if (ANY_NOT_FOUND)
    message(WARNING "Build cmake target Dependencies to get all dependencies. Then configure cmake again to enable GlueLibrary and GlueExample targets. Exiting")
    return()
endif ()


# Engine libraries
add_subdirectory(Engine)


# Examples
add_subdirectory(Example)


# Template project
#add_subdirectory(Project)


# package helper target
add_custom_target(BuildAssets ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating assets and programs zip package..."
        USES_TERMINAL
        )

include(Package)

add_custom_target(BuildPackage
        DEPENDS BuildAssets
        COMMAND ${CMAKE_COMMAND} --build . --target package --config ${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building package..."
        USES_TERMINAL
        )

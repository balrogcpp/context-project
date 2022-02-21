# This source file is part of "glue project". Created by Andrey Vasiliev

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
include(GitHash)
include(CMakeDependentOption)

# Create artifacts directory
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/Tmp)
make_directory(${GLUE_ARTIFACT_DIR})
make_directory(${GLUE_TMP_DIR})

# Insert dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty)

# Doxygen
include(Doxygen)

set(CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})

set(CMAKE_FOLDER DockerCompose)

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
        COMMENT "Building tests with docker..."
        USES_TERMINAL
        )

set(CMAKE_FOLDER "")

add_subdirectory(${CMAKE_SOURCE_DIR}/Source)

#Packaging
include(Package)

add_custom_target(BuildPackage
        DEPENDS BuildAssets
        COMMAND ${CMAKE_COMMAND} --build . --target package --config ${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Build packages..."
        USES_TERMINAL
        )

# env variable ANDROID_SDK_ROOT must be set manually
if (NOT RELEASE)
    set(GRADLE_TARGET assembleDebug)
    set(APK_FILE ${CMAKE_SOURCE_DIR}/Android/app/build/outputs/apk/debug/app-arm64-v8a-debug.apk)
    set(OUTPUT_FILE ${GLUE_PACKAGE_NAME}_Android_aarch64_Debug_${GIT_SHA1}.apk)
else ()
    set(GRADLE_TARGET assembleRelease)
    set(APK_FILE ${CMAKE_SOURCE_DIR}/Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk)
    set(OUTPUT_FILE ${GLUE_PACKAGE_NAME}_Android_aarch64_${GIT_SHA1}.apk)
endif ()

add_custom_target(BuildAndroid
        DEPENDS BuildAssets
        COMMAND ./gradlew ${GRADLE_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy ${APK_FILE} ${GLUE_ARTIFACT_DIR}/${OUTPUT_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Android
        COMMENT "Build packages..."
        USES_TERMINAL
        )

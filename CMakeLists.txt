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
include(Platform)
include(CMakeDependentOption)
include(InsertDependency)


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
set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/Tmp)
make_directory(${GLUE_ARTIFACT_DIR})
make_directory(${GLUE_TMP_DIR})


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
list(APPEND CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})


if (APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    insert_dependency(X11)
endif ()
if (DESKTOP)
    insert_dependency(OpenGL)
else ()
    insert_dependency(OpenGLES2)
endif ()
insert_dependency(Threads)
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
    add_library(AndroidDummy SHARED Engine/Android/app/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy Dependencies)
endif ()


# Engine libraries
add_subdirectory(Engine)


# Examples
add_subdirectory(Example)


# Template project
#add_subdirectory(Project)


# assets helper target
add_custom_target(BuildAssets ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating assets and programs zip package..."
        USES_TERMINAL
        )

# Helper targets
if (NOT GIT_SHA1)
    execute_process(
            COMMAND git rev-parse --short=8 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

# Docker helper target
set(CMAKE_FOLDER Docker)

add_custom_target(DockerComposeDep
        COMMAND docker-compose -f docker-compose-dependencies.yml up
        COMMAND docker-compose -f docker-compose-dependencies.yml down --volumes
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


# package helper target
include(CMakeDependentOption)

set(GLUE_PACKAGE_NAME "GlueSample")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT}_${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()
if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Install/${GLUE_ARTIFACT_NAME})
endif ()
install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
install(FILES ${GLUE_TMP_DIR}/Assets.zip DESTINATION .)
install(DIRECTORY ${GLUE_TMP_DIR}/Programs DESTINATION .)

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
    set(CPACK_GENERATOR "ZIP")
endif ()


find_package(NSIS QUIET)


if (${NSIS_FOUND} AND WIN32)
    message(STATUS "NSIS found. MSI installer will be created")
    list(APPEND CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_CREATE_DESKTOP_LINKS Sample)
    set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
    if (CMAKE_SIZEOF_VOID_P STREQUAL 8)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    elseif (CMAKE_SIZEOF_VOID_P STREQUAL 4)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES32")
    endif ()
else ()
    message(STATUS "NSIS not found")
endif ()

include(CPack)


add_custom_target(BuildPackage
        DEPENDS BuildAssets
        COMMAND ${CMAKE_COMMAND} --build . --target package --config ${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building package..."
        USES_TERMINAL
        )

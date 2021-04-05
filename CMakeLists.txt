#MIT License
#
#Copyright (c) 2021 Andrew Vasiliev
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
enable_language(C)
enable_language(CXX)

project(xio CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")


include(CheckTypeSize)
include(ProcessorCount)

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)

if (NOT ANDROID)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif ()
    if (NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(SEND_ERROR "Only Release/RelWithDebInfo/Debug build are available")
    endif ()
    if (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(SEND_ERROR "Debug build is not working with MSVC right now")
    endif()
endif ()

if (PTR_SIZE EQUAL 8)
    set(CONTEXT_PLATFORM "x64" CACHE STRING "")
elseif (PTR_SIZE EQUAL 4)
    set(CONTEXT_PLATFORM "x32" CACHE STRING "")
endif ()
if (WIN32)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "windows")
elseif (ANDROID)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "android")
elseif (APPLE)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "apple")
elseif (UNIX)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "linux")
endif ()

if (MINGW)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-mingw")
elseif (MSVC)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-msvc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-gcc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-clang")
else ()
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${CMAKE_CXX_COMPILER_ID}")
endif ()

set(CONTEXT_EXTERNAL_TOOLCHAIN ${CONTEXT_EXTERNAL_TOOLCHAIN} CACHE STRING "")
set(CONTEXT_EXTERNAL_PLATROFM ${CONTEXT_PLATFORM} CACHE STRING "")
set(CONTEXT_FORCE_PLATFORM "" CACHE STRING "")
set(CONTEXT_BUILD_PLATFORM ${CONTEXT_PLATFORM})

processorcount(CONTEXT_PROCESSOR_COUNT)
set(CMAKE_BUILD_PARALLEL_LEVEL ${CONTEXT_PROCESSOR_COUNT})
set(CONTEXT_MAKE make -j${CONTEXT_PROCESSOR_COUNT} CACHE STRING "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT ANDROID)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    endif ()

    # Set minimum supported Windows version to Win7
    if (MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows -DWINVER=0x0601 -D_WIN32_WINNT=0x0601")
        string(APPEND CMAKE_C_FLAGS " -mwindows -DWINVER=0x0601 -D_WIN32_WINNT=0x0601")
    endif ()
    # With PIE most of linuxes don't recognize binary
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -msse4.1 -funroll-loops -pthread")
    string(APPEND CMAKE_C_FLAGS " -msse4.1 -funroll-loops -pthread")

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
    else ()
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
    endif ()
elseif(ANDROID AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    string(APPEND CMAKE_CXX_FLAGS " -Ofast -DNDEBUG -funroll-loops -pthread")
    string(APPEND CMAKE_C_FLAGS " -Ofast -DNDEBUG -funroll-loops -pthread")

    string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
elseif (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)


    string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp  /D_WIN32_WINNT=0x0601 /DWINVER=0x0601")
    string(APPEND CMAKE_C_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp  /D_WIN32_WINNT=0x0601 /DWINVER=0x0601")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /fp:fast /GL /D \"WINAPI_MAIN_FUNC\"")
        string(APPEND CMAKE_C_FLAGS " /O2 /fp:fast /GL /D \"WINAPI_MAIN_FUNC\"")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF /SUBSYSTEM:WINDOWS")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_C_FLAGS " /D \"DEBUG\"")
    endif ()
endif ()

if (ANDROID)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${ANDROID_ABI}")
    set(CONTEXT_BUILD_PLATFORM ${ANDROID_ABI})
else ()
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${CONTEXT_BUILD_PLATFORM}")
endif ()

set(CONTEXT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "")
set(CONTEXT_SOURCE_DIR ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT_BUILD_PLATFORM})
set(CONTEXT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CONTEXT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/xio CACHE PATH "")
set(CONTEXT_INCLUDE_DIR ${CONTEXT_SOURCE_DIR}/include CACHE PATH "")
set(CONTEXT_THIRDPARTY_ROOT ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_THIRDPARTY_DIR ${CONTEXT_THIRDPARTY_ROOT} CACHE PATH "")

add_subdirectory(${CONTEXT_ROOT_DIR}/dependencies ${CONTEXT_WORK_DIR}/dependencies)

add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)


set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
set(CMAKE_FIND_ROOT_PATH "${ANDROID_NDK};${CMAKE_FIND_ROOT_PATH};${CONTEXT_EXTERNAL_INSTALL_LOCATION}")

if (NOT ANDROID)
    find_package(OpenGL REQUIRED)
else ()
    find_package(OpenGLES2 REQUIRED)
endif ()


find_package(Threads REQUIRED)

find_package(OGRE QUIET)
find_package(Bullet QUIET)
find_package(SDL2 QUIET)
find_package(nlohmann_json QUIET)
find_package(OpenAL QUIET)
find_package(Ogg QUIET)
find_package(sol2 QUIET)
find_package(ZLIB QUIET)
find_package(Lua51 QUIET)
find_package(assimp QUIET)
find_package(PNG QUIET)
find_package(freetype QUIET)
#find_library(ASSIMP_IRRXML_LIBRARY IrrXML)

if (NOT ANDROID)
    if (NOT OGRE_FOUND OR NOT Bullet_FOUND OR NOT sol2_FOUND OR NOT OpenAL_FOUND OR NOT SDL2_FOUND OR NOT PNG_FOUND)
        message("Please build dependencies before executable")
        return()
    endif ()
else ()
    if (NOT OGRE_FOUND OR NOT Bullet_FOUND OR NOT sol2_FOUND OR NOT OpenAL_FOUND OR NOT SDL2_FOUND OR NOT PNG_FOUND)
        add_library(target-deps-dummy SHARED android/app/src/main/cpp/main.cpp)
        add_dependencies(target-deps-dummy target-deps)
        return()
    endif ()
endif ()


list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIR} ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include
                                    ${BULLET_INCLUDE_DIR} ${OGRE_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
                                    ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2 ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES
                                    ${OGRE_INCLUDE_DIRS}/Plugins/Assimp)

file(GLOB_RECURSE SOURCE_FILES demo/*.cpp demo/*.h demo/*.hpp sources/*.cpp sources/*.h sources/*.hpp)

if (NOT ANDROID)
    add_executable(demo ${SOURCE_FILES})
else ()
    add_library(demo SHARED ${SOURCE_FILES})
    set_target_properties(demo PROPERTIES OUTPUT_NAME "main")
endif ()

include_directories(demo demo/include ${CONTEXT_INCLUDE_DIRS})

target_link_directories(demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})


if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (UNIX AND NOT ANDROID)
    #jemalloc fix problems with memory bloat on linux
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl jemalloc.a)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES SDL2::hidapi android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES zlibstatic winmm Version imm32 Setupapi)
elseif (APPLE)
    list(APPEND SYSTEM_LIBRARIES)
endif ()


target_link_libraries(demo PUBLIC ${OGRE_LIBRARIES} ${ASSIMP_IRRXML_LIBRARY} ${BULLET_LIBRARIES}
                    ${PNG_LIBRARIES} vorbisfile vorbis vorbisenc Ogg::ogg pugixml SDL2::SDL2-static
                    ${LUA_LIBRARIES} ${OPENAL_LIBRARY} ${SYSTEM_LIBRARIES})

target_precompile_headers(demo PUBLIC sources/include/pcheader.h)

install(TARGETS demo RUNTIME DESTINATION .)

file(GLOB_RECURSE SOURCE_FILES sources/*.cpp sources/*.h sources/*.hpp)

# include_directories(lib-static ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})
# add_library(lib-static STATIC ${SOURCE_FILES} )
# set_target_properties(lib-static PROPERTIES OUTPUT_NAME "xio-static")
# target_precompile_headers(lib-static PUBLIC sources/include/pcheader.h)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")

if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin/*.dll)
elseif (UNIX)
    #file(GLOB EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/*.so*)
endif ()

if (NOT ANDROID)
    file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/config.json ${EXTERNAL_DLL_GLOB} ${OPENAL_LIBRARY} DESTINATION . FOLLOW_SYMLINK_CHAIN)
endif ()

find_package(Python3 QUIET)

if (NOT Python3_Interpreter_FOUND)
    return()
endif ()

if (NOT GIT_SHA1)
    execute_process(
            COMMAND git rev-parse --short=8 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

set(CONTEXT_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "")
set(CONTEXT_ZIP_NAME ContextProjectDemo-${GIT_SHA1} CACHE PATH "")
set(CMAKE_INSTALL_PREFIX ${CONTEXT_INSTALL_PATH}/${CONTEXT_ZIP_NAME})

set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
set(CMAKE_FIND_ROOT_PATH "${ANDROID_NDK};${CMAKE_FIND_ROOT_PATH};${CONTEXT_EXTERNAL_INSTALL_LOCATION}")


install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.json ${EXTERNAL_DLL_GLOB}
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tmp/assets ${CMAKE_SOURCE_DIR}/tmp/programs
        DESTINATION .
        )

if (UNIX)
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/start.sh
            PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE
            DESTINATION .
            )
endif()

if (WIN32)
    set(CONTEXT_TARGET_OS "Windows")
elseif (UNIX)
    set(CONTEXT_TARGET_OS "Linux")
elseif (ANDROID)
    set(CONTEXT_TARGET_OS "Android")
endif ()

set(CONTEXT_PACKAGE_NAME "ContextProjectDemo")
set(CONTEXT_ARTIFACT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/artifacts)
set(CONTEXT_ARTIFACT_NAME ${CONTEXT_PACKAGE_NAME}-${CONTEXT_TARGET_OS}-${CONTEXT_BUILD_PLATFORM}-${GIT_SHA1})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${CONTEXT_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})

set(CPACK_GENERATOR "ZIP")

include(CPack)

#TODO tests
add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)

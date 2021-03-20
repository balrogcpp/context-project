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

project(xio)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

enable_language(C)
enable_language(CXX)
include(CheckTypeSize)
include(ProcessorCount)

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)

if (NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(SEND_ERROR "Only Release and RelWithDebInfo build are available")
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
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-gcc")
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
    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

    if (MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -msse4.1 -funroll-loops -pthread")
    string(APPEND CMAKE_C_FLAGS " -msse4.1 -funroll-loops -pthread")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -fopenmp=libgomp")
        string(APPEND CMAKE_C_FLAGS " -fopenmp=libgomp")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " -fopenmp")
        string(APPEND CMAKE_C_FLAGS " -fopenmp")
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_CXX_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_C_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
    else ()
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin -fuse-ld=lld")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin -fuse-ld=lld")
    endif ()
elseif (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp")
    string(APPEND CMAKE_C_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp")

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

if (CONTEXT_ONLY_DEPS)
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

if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(SEND_ERROR "In-source builds are not allowed!")
endif ()

set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})

find_package(Python3 REQUIRED)
find_package(OGRE QUIET)
find_package(Bullet QUIET)
find_package(SDL2 QUIET)
find_package(OpenAL QUIET)
find_package(Lua QUIET)
find_package(yaml-cpp QUIET)
find_package(sol2 QUIET)
find_package(ZLIB QUIET)
find_package(Lua51 QUIET)
find_package(ZZip QUIET)
find_package(assimp QUIET)
find_library(ASSIMP_IRRXML_LIBRARY IrrXML)

if (NOT OGRE_FOUND OR NOT Bullet_FOUND OR NOT sol2_FOUND OR NOT OpenAL_FOUND OR NOT SDL2_FOUND)
    return()
endif ()

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic opengl32 imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (UNIX AND NOT ANDROID)
    list(APPEND SYSTEM_LIBRARIES z.a OpenGL GLX X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES z.a log android EGL GLESv2 GLESv1_CM OpenSLES dl m.a hidapi ndk_compat omp)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES zlibstatic opengl32 winmm Version imm32 Setupapi)
elseif (APPLE)
    list(APPEND SYSTEM_LIBRARIES zlibstatic)
endif ()

list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIR} ${CONTEXT_THIRDPARTY_ROOT}/btogre/include ${CONTEXT_THIRDPARTY_ROOT}/oggsound/include
                                    ${CONTEXT_THIRDPARTY_ROOT}/pgeometry/include ${CONTEXT_THIRDPARTY_ROOT}/gorilla/include ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include
                                    ${SOL2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
                                    ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2 ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES ${OGRE_INCLUDE_DIRS}/Plugins/Assimp)

file(GLOB_RECURSE SOURCE_FILES demo/*.cpp demo/*.h sources/*.cpp sources/*.h)

if (ANDROID)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()

if (NOT ANDROID)
    add_executable(demo ${SOURCE_FILES})
else ()
    add_library(demo SHARED ${SOURCE_FILES})
endif ()

include_directories(demo demo/include ${CONTEXT_INCLUDE_DIRS} ${ANDROID_NDK}/sources/android/native_app_glue)
target_link_directories(demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})
target_link_libraries(demo PUBLIC ${OGRE_LIBRARIES} ${ASSIMP_IRRXML_LIBRARY} ${BULLET_LIBRARIES}
                            vorbisfile vorbis vorbisenc ogg pugixml freetype ${SDL2_LIBRARIES}
                            ${LUA_LIBRARIES} ${OPENAL_LIBRARY} ${YAML_CPP_LIBRARIES} ${ZZip_LIBRARIES} ${SYSTEM_LIBRARIES})

target_precompile_headers(demo PUBLIC sources/include/pcheader.h)

install(TARGETS demo RUNTIME DESTINATION .)

file(GLOB_RECURSE SOURCE_FILES sources/*.cpp sources/*.h)

# include_directories(lib-static ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})
# add_library(lib-static STATIC ${SOURCE_FILES} )
# set_target_properties(lib-static PROPERTIES OUTPUT_NAME "xio-static")
# target_precompile_headers(lib-static PUBLIC sources/include/pcheader.h)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")

if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin/OpenAL32.dll)
elseif (UNIX)
    #file(GLOB EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/*.so*)
endif ()

file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/config.yaml ${EXTERNAL_DLL_GLOB} ${OPENAL_LIBRARY} DESTINATION . FOLLOW_SYMLINK_CHAIN)

install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.yaml ${EXTERNAL_DLL_GLOB}
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

add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)
add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)

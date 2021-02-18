#MIT License
#
#Copyright (c) 2020 Andrey Vasiliev
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
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

enable_language(C)
enable_language(CXX)
include(CheckTypeSize)
include(ProcessorCount)

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)

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
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    elseif ()
        string(APPEND CMAKE_CXX_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
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

    string(APPEND CONTEXT_CXX_FLAGS " -DSDL_MAIN_HANDLED -DRAPIDJSON_HAS_STDSTRING")
    string(APPEND CONTEXT_C_FLAGS " -DSDL_MAIN_HANDLED -DRAPIDJSON_HAS_STDSTRING")

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

    string(APPEND CMAKE_CXX_FLAGS " /O2 /MP /MD /GS- /GL /fp:fast /openmp /D \"_ENABLE_EXTENDED_ALIGNED_STORAGE\" /D \"RAPIDJSON_HAS_STDSTRING\"")
    string(APPEND CMAKE_C_FLAGS " /O2 /MP /MD /GS- /GL /fp:fast /openmp /D \"_ENABLE_EXTENDED_ALIGNED_STORAGE\" /D \"RAPIDJSON_HAS_STDSTRING\"")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /OPT:REF /OPT:ICF /LTCG /SUBSYSTEM:WINDOWS")
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

option(CONTEXT_MAKEFILE_VERBOSE "Verbose mode for make" false)
if (CONTEXT_MAKEFILE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE true)
endif ()

add_subdirectory(${CONTEXT_ROOT_DIR}/sources ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME})
add_subdirectory(${CONTEXT_ROOT_DIR}/demo ${CMAKE_CURRENT_BINARY_DIR}/demo)

add_dependencies(demo lib-static)

add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")

if (WIN32)
    set(EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin/OpenAL32.dll)
endif ()

add_custom_target(zip-deps
        COMMAND ${CMAKE_COMMAND} -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} -P CMake/zipdir.cmake
        WORKING_DIRECTORY ${CONTEXT_ROOT_DIR}
        COMMENT "Zipping dependencies..."
        VERBATIM
        )

install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.yaml ${EXTERNAL_DLL_GLOB} ${OTHER_DLL_GLOB}
        CONFIGURATIONS Release
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )

install(FILES ${CMAKE_SOURCE_DIR}/deploy/start.sh
        CONFIGURATIONS Release
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )

file(GLOB CONTEXT_MEDIA_FILES ${CMAKE_SOURCE_DIR}/tmp/*)
install(DIRECTORY ${CONTEXT_MEDIA_FILES}
        CONFIGURATIONS Release
        DESTINATION .
        )

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

set(CPACK_NSIS_DISPLAY_NAME "Context Demo")
set(CPACK_NSIS_PACKAGE_NAME "Context Demo")
set(CPACK_NSIS_MENU_LINKS "demo.exe" "ContextProjectDemo")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL true)

set(CPACK_GENERATOR "ZIP")

include(CPack)

add_custom_target(install-zip
        COMMAND ${CMAKE_COMMAND} -E tar cf ${CONTEXT_ZIP_NAME}.zip --format=zip ${CONTEXT_ZIP_NAME}
        WORKING_DIRECTORY ${CONTEXT_INSTALL_PATH}
        COMMENT "Create zip..."
        VERBATIM
        )

add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)

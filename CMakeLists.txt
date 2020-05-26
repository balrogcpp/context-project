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
#

cmake_minimum_required(VERSION 3.15.1)

# The project version number.
set(PROJECT_VERSION_MAJOR 0 CACHE STRING "Project major version number.")
set(PROJECT_VERSION_MINOR 1 CACHE STRING "Project minor version number.")
set(PROJECT_VERSION_PATCH 0 CACHE STRING "Project patch version number.")
mark_as_advanced(PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH)

project(context)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
message("CMake project configured with build type ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/CMake"
    )

set(CMAKE_CXX_STANDARD 17)

enable_language(C)
enable_language(CXX)

include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckCSourceCompiles)
include(CheckStructHasMember)
include(CMakeDependentOption)
include(CheckIncludeFileCXX)
include(CheckTypeSize)
include(TestBigEndian)
include(ProcessorCount)
include(GetGitRevisionDescription)

test_big_endian(CONTEXT_BIG_ENDIAN)
if (${CONTEXT_BIG_ENDIAN})
    message(SEND_ERROR "Big endian systemd not supported!")
endif ()

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)

if (PTR_SIZE EQUAL 8)
    set(CONTEXT_PLATFORM "x64" CACHE STRING "")
elseif (PTR_SIZE EQUAL 4)
    set(CONTEXT_PLATFORM "x86" CACHE STRING "")
endif ()

if (WIN32)
    set(CONTEXT_OS "Windows")
elseif (UNIX)
    set(CONTEXT_OS "Linux")
elseif (ANDROID)
    set(CONTEXT_OS "Android")
endif ()

if (MINGW)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-mingw")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-gnu")
elseif (MSVC)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-msvc")
else ()
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${CMAKE_CXX_COMPILER_ID}")
endif ()

set(CONTEXT_EXTERNAL_PLATROFM ${CONTEXT_PLATFORM} CACHE STRING "")

set(CONTEXT_FORCE_PLATFORM "" CACHE STRING "")
set(CONTEXT_BUILD_PLATFORM ${CONTEXT_PLATFORM})

processorcount(CONTEXT_PROCESSOR_COUNT)
set(CMAKE_BUILD_PARALLEL_LEVEL ${CONTEXT_PROCESSOR_COUNT})
set(CONTEXT_MAKE make -j${CONTEXT_PROCESSOR_COUNT} CACHE STRING "")

set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif ()

if (UNIX OR MINGW)
    set(CONTEXT_GCC_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")
    set(CONTEXT_GCC_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
    set(CONTEXT_GCC_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} CACHE STRING "")
    set(CONTEXT_GCC_STATIC_LINKER_FLAGS ${CMAKE_STATIC_LINKER_FLAGS} CACHE STRING "")
    set(CONTEXT_GCC_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} CACHE STRING "")

    if (CONTEXT_FORCE_PLATFORM STREQUAL "x64")
        set(CONTEXT_BUILD_PLATFORM ${CONTEXT_FORCE_PLATFORM})
    elseif (CONTEXT_FORCE_PLATFORM STREQUAL "x86")
        set(CONTEXT_BUILD_PLATFORM ${CONTEXT_FORCE_PLATFORM})
    else ()
        if (CONTEXT_BUILD_PLATFORM STREQUAL "x64")
            string(APPEND CONTEXT_GCC_CXX_FLAGS " -m64")
            string(APPEND CONTEXT_GCC_C_FLAGS " -m64")
        elseif (CONTEXT_BUILD_PLATFORM STREQUAL "x86")
            string(APPEND CONTEXT_GCC_CXX_FLAGS " -m32")
            string(APPEND CONTEXT_GCC_C_FLAGS " -m32")
        else ()
            message(SEND_ERROR "Platform is not supported!")
        endif ()
    endif ()

    set(CONTEXT_GCC_CXX_Debug_FLAGS "${CONTEXT_GCC_CXX_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS "${CONTEXT_GCC_CXX_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_CXX_Release_FLAGS "${CONTEXT_GCC_CXX_FLAGS}" CACHE STRING "")

    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -g")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -O0")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -std=c++17")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_CXX_Debug_FLAGS " -fopenmp")

    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -g")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -O3")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -std=c++17")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -ffast-math")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -mavx")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -funroll-loops --param max-unroll-times=4")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -ftree-parallelize-loops=8 -floop-parallelize-all")
    # string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -fpack-struct=1")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_CXX_RelWithDebInfo_FLAGS " -fopenmp")

    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -O3")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -std=c++17")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -flto")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -ffast-math")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -mavx")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -funroll-loops --param max-unroll-times=4")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -ftree-parallelize-loops=8 -floop-parallelize-all")
    # string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -fpack-struct=1")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_CXX_Release_FLAGS " -fopenmp")

    set(CONTEXT_GCC_C_Debug_FLAGS "${CONTEXT_GCC_C_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_C_RelWithDebInfo_FLAGS "${CONTEXT_GCC_C_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_C_Release_FLAGS "${CONTEXT_GCC_C_FLAGS}" CACHE STRING "")

    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -g")
    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -O0")
    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_C_Debug_FLAGS " -fopenmp")

    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -g")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -O3")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -ffast-math")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -mavx")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -funroll-loops --param max-unroll-times=4")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -ftree-parallelize-loops=8 -floop-parallelize-all")
    # string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -fpack-struct=1")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_C_RelWithDebInfo_FLAGS " -fopenmp")

    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -O3")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -ffast-math")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -mavx")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -funroll-loops --param max-unroll-times=4")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -ftree-parallelize-loops=8 -floop-parallelize-all")
    # string(APPEND CONTEXT_GCC_C_Release_FLAGS " -fpack-struct=1")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -Wno-unused-variable")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -pthread")
    string(APPEND CONTEXT_GCC_C_Release_FLAGS " -fopenmp")

    set(CONTEXT_GCC_EXE_LINKER_Debug_FLAGS "${CONTEXT_GCC_EXE_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_EXE_LINKER_RelWithDebInfo_FLAGS "${CONTEXT_GCC_EXE_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_EXE_LINKER_Release_FLAGS "${CONTEXT_GCC_EXE_LINKER_FLAGS}" CACHE STRING "")

    string(APPEND CONTEXT_GCC_EXE_LINKER_Debug_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Debug_FLAGS " -no-pie")
    string(APPEND CONTEXT_GCC_EXE_LINKER_RelWithDebInfo_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_EXE_LINKER_RelWithDebInfo_FLAGS " -static-libgcc -static-libstdc++")
    string(APPEND CONTEXT_GCC_EXE_LINKER_RelWithDebInfo_FLAGS " -no-pie")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Release_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Release_FLAGS " -fwhole-program")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Release_FLAGS " -fuse-linker-plugin")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Release_FLAGS " -static-libgcc -static-libstdc++")
    string(APPEND CONTEXT_GCC_EXE_LINKER_Release_FLAGS " -no-pie")

    set(CONTEXT_GCC_STATIC_LINKER_Debug_FLAGS "${CONTEXT_GCC_STATIC_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_STATIC_LINKER_RelWithDebInfo_FLAGS "${CONTEXT_GCC_STATIC_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_STATIC_LINKER_Release_FLAGS "${CONTEXT_GCC_STATIC_LINKER_FLAGS}" CACHE STRING "")

    set(CONTEXT_GCC_SHARED_LINKER_Debug_FLAGS "${CONTEXT_GCC_SHARED_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_SHARED_LINKER_RelWithDebInfo_FLAGS "${CONTEXT_GCC_SHARED_LINKER_FLAGS}" CACHE STRING "")
    set(CONTEXT_GCC_SHARED_LINKER_Release_FLAGS "${CONTEXT_GCC_SHARED_LINKER_FLAGS}" CACHE STRING "")

    string(APPEND CONTEXT_GCC_SHARED_LINKER_Debug_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_RelWithDebInfo_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_RelWithDebInfo_FLAGS " -static-libgcc -static-libstdc++")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_Release_FLAGS " -Wall")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_Release_FLAGS " -fwhole-program")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_Release_FLAGS " -fuse-linker-plugin")
    string(APPEND CONTEXT_GCC_SHARED_LINKER_Release_FLAGS " -static-libgcc -static-libstdc++")

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(CONTEXT_CXX_FLAGS ${CONTEXT_GCC_CXX_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_C_FLAGS ${CONTEXT_GCC_C_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_EXE_LINKER_FLAGS ${CONTEXT_GCC_EXE_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_STATIC_LINKER_FLAGS ${CONTEXT_GCC_STATIC_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_SHARED_LINKER_FLAGS ${CONTEXT_GCC_SHARED_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        string(APPEND CONTEXT_Clang_C_Debug_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_C_Debug_FLAGS " -fopenmp")
        string(APPEND CONTEXT_Clang_C_RelWithDebInfo_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_C_RelWithDebInfo_FLAGS " -fopenmp")
        string(APPEND CONTEXT_Clang_C_Release_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_C_Release_FLAGS " -fopenmp")

        string(APPEND CONTEXT_Clang_CXX_Debug_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_CXX_Debug_FLAGS " -fopenmp")
        string(APPEND CONTEXT_Clang_CXX_Debug_FLAGS " -std=c++17")
        string(APPEND CONTEXT_Clang_CXX_RelWithDebInfo_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_CXX_RelWithDebInfo_FLAGS " -fopenmp")
        string(APPEND CONTEXT_Clang_CXX_RelWithDebInfo_FLAGS " -std=c++17")
        string(APPEND CONTEXT_Clang_CXX_Release_FLAGS " -pthread")
        string(APPEND CONTEXT_Clang_CXX_Release_FLAGS " -fopenmp")
        string(APPEND CONTEXT_Clang_CXX_Release_FLAGS " -std=c++17")

        #    string(APPEND CONTEXT_Clang_STATIC_LINKER_Debug_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_STATIC_LINKER_RelWithDebInfo_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_STATIC_LINKER_Release_FLAGS " -lstdc++fs")

        #    string(APPEND CONTEXT_Clang_SHADER_LINKER_Debug_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_SHADER_LINKER_RelWithDebInfo_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_SHADER_LINKER_Release_FLAGS " -lstdc++fs")

        #    string(APPEND CONTEXT_Clang_EXE_LINKER_Debug_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_EXE_LINKER_RelWithDebInfo_FLAGS " -lstdc++fs")
        #    string(APPEND CONTEXT_Clang_EXE_LINKER_Release_FLAGS " -lstdc++fs")

        set(CONTEXT_CXX_FLAGS ${CONTEXT_Clang_CXX_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_C_FLAGS ${CONTEXT_Clang_C_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_EXE_LINKER_FLAGS ${CONTEXT_Clang_EXE_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_STATIC_LINKER_FLAGS ${CONTEXT_Clang_STATIC_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
        set(CONTEXT_SHARED_LINKER_FLAGS ${CONTEXT_CLang_SHARED_LINKER_${CMAKE_BUILD_TYPE}_FLAGS} CACHE STRING "")
    endif ()

elseif (MSVC)
    set(CONTEXT_MSVC_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")
    set(CONTEXT_MSVC_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
    set(CONTEXT_MSVC_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} CACHE STRING "")
    set(CONTEXT_MSVC_STATIC_LINKER_FLAGS ${CMAKE_STATIC_LINKER_FLAGS} CACHE STRING "")
    set(CONTEXT_MSVC_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} CACHE STRING "")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(REGEX REPLACE "/W[0-4]" "/Wall" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/O[0-4]" "/O2" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/fp:[a-zA-Z]*" "/fp:fast" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/GS" "/GS-" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /GS-")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /MP")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /MD")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /GL")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /arch:AVX")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /openmp")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /fp:fast")
        # string(APPEND CONTEXT_MSVC_CXX_FLAGS " /Zp1")
        string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" CONTEXT_MSVC_EXE_LINKER_FLAGS ${CONTEXT_MSVC_EXE_LINKER_FLAGS})
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /OPT:REF")
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /OPT:ICF")
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /LTCG")
        string(APPEND CONTEXT_MSVC_STATIC_LINKER_FLAGS " /LTCG")
        string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" CONTEXT_MSVC_SHARED_LINKER_FLAGS ${CONTEXT_MSVC_SHARED_LINKER_FLAGS})
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /OPT:REF")
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /OPT:ICF")
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /LTCG")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        string(REGEX REPLACE "/W[0-4]" "/Wall" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/O[0-4]" "/O2" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/fp:[a-zA-Z]*" "/fp:fast" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /GS-")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /MP")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /GL")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /arch:AVX")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /openmp")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /fp:fast")
        # string(APPEND CONTEXT_MSVC_CXX_FLAGS " /Zp1")
        string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" CONTEXT_MSVC_EXE_LINKER_FLAGS ${CONTEXT_MSVC_EXE_LINKER_FLAGS})
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /OPT:REF")
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /OPT:ICF")
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " /LTCG")
        string(APPEND CONTEXT_MSVC_EXE_LINKER_FLAGS " \"Dbghelp.lib\"")
        string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" CONTEXT_MSVC_STATIC_LINKER_FLAGS ${CONTEXT_MSVC_STATIC_LINKER_FLAGS})
        string(APPEND CONTEXT_MSVC_STATIC_LINKER_FLAGS " /LTCG")
        string(APPEND CONTEXT_MSVC_STATIC_LINKER_FLAGS " \"Dbghelp.lib\"")
        string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" CONTEXT_MSVC_SHARED_LINKER_FLAGS ${CONTEXT_MSVC_SHARED_LINKER_FLAGS})
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /OPT:REF")
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /OPT:ICF")
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " /LTCG")
        string(APPEND CONTEXT_MSVC_SHARED_LINKER_FLAGS " \"Dbghelp.lib\"")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        string(REGEX REPLACE "/W[0-4]" "/W3" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/O[0-4]" "/O0" CONTEXT_MSVC_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /openmp")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /MP")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /MD")
    endif ()

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(CONTEXT_CXX_FLAGS ${CONTEXT_MSVC_CXX_FLAGS} CACHE STRING "")
        set(CONTEXT_C_FLAGS ${CONTEXT_MSVC_C_FLAGS} CACHE STRING "")
        set(CONTEXT_EXE_LINKER_FLAGS ${CONTEXT_MSVC_EXE_LINKER_FLAGS} CACHE STRING "")
        set(CONTEXT_STATIC_LINKER_FLAGS ${CONTEXT_MSVC_STATIC_LINKER_FLAGS} CACHE STRING "")
        set(CONTEXT_SHARED_LINKER_FLAGS ${CONTEXT_MSVC_SHARED_LINKER_FLAGS} CACHE STRING "")
    endif ()
endif ()

if (MINGW)
    string(APPEND CONTEXT_CXX_FLAGS " -DWIN32")
endif ()

set(CONTEXT_EXTERNAL_CXX_FLAGS ${CONTEXT_CXX_FLAGS} CACHE STRING "")
set(CONTEXT_EXTERNAL_C_FLAGS ${CONTEXT_C_FLAGS} CACHE STRING "")
set(CONTEXT_EXTERNAL_EXE_LINKER_FLAGS ${CONTEXT_EXE_LINKER_FLAGS} CACHE STRING "")
set(CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS ${CONTEXT_STATIC_LINKER_FLAGS} CACHE STRING "")
set(CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS ${CONTEXT_SHARED_LINKER_FLAGS} CACHE STRING "")

if (MSVC)
    string(APPEND CONTEXT_CXX_FLAGS " /D \"_ENABLE_EXTENDED_ALIGNED_STORAGE\"")
    # string(APPEND CONTEXT_CXX_FLAGS " /D \"_ENFORCE_MATCHING_ALLOCATORS=0\"")
    string(APPEND CONTEXT_CXX_FLAGS " /D \"SDL_MAIN_HANDLED\"")
    string(APPEND CONTEXT_CXX_FLAGS " /D \"CEGUI_STATIC\"")
    string(APPEND CONTEXT_CXX_FLAGS " /D \"PCRE_STATIC\"")
    string(APPEND CONTEXT_CXX_FLAGS " /D \"ZLIB_DLL\"")
    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CONTEXT_EXE_LINKER_FLAGS " /SUBSYSTEM:WINDOWS")
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /D \"NDEBUG\"")
    else ()
        string(APPEND CONTEXT_MSVC_CXX_FLAGS " /D \"DEBUG\"")
    endif ()
else ()
    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        if (MINGW)
            string(APPEND CONTEXT_CXX_FLAGS " -mwindows")
        endif ()
        string(APPEND CONTEXT_CXX_FLAGS " -DNDEBUG")
    else ()
        string(APPEND CONTEXT_CXX_FLAGS " -DDEBUG")
    endif ()

    string(APPEND CONTEXT_CXX_FLAGS " -DSDL_MAIN_HANDLED")
    string(APPEND CONTEXT_CXX_FLAGS " -DCEGUI_STATIC")
    string(APPEND CONTEXT_CXX_FLAGS " -DPCRE_STATIC")
    #    string(APPEND CONTEXT_CXX_FLAGS " -DZLIB_DLL")
    string(APPEND CONTEXT_CXX_FLAGS " -DRAPIDJSON_HAS_STDSTRING")
endif ()

set(CONTEXT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "")
set(CONTEXT_SOURCE_DIR ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT_BUILD_PLATFORM})
set(CONTEXT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CONTEXT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/context CACHE PATH "")
set(CONTEXT_INCLUDE_DIR ${CONTEXT_SOURCE_DIR}/context/include CACHE PATH "")

get_git_head_revision(GIT_REFSPEC GIT_SHA1)
set(CONTEXT_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "")
set(CONTEXT_ZIP_NAME ContextProjectDemo-${CMAKE_BUILD_TYPE}-${GIT_SHA1} CACHE PATH "")
set(CMAKE_INSTALL_PREFIX ${CONTEXT_INSTALL_PATH}/${CONTEXT_ZIP_NAME})

if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(SEND_ERROR "In-source builds are not allowed!")
endif ()

set(CONTEXT_THIRDPARTY_ROOT ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_THIRDPARTY_DIR ${CONTEXT_THIRDPARTY_ROOT} CACHE PATH "")

add_subdirectory(${CONTEXT_ROOT_DIR}/thirdparty ${CONTEXT_WORK_DIR}/thirdparty)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "RelWithDebInfo" "Debug")
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "CMake build type")

option(CONTEXT_MAKEFILE_VERBOSE "Verbose mode for make" false)
if (CONTEXT_MAKEFILE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE true)
endif ()

add_subdirectory(${CONTEXT_ROOT_DIR}/sources ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME})
add_subdirectory(${CONTEXT_ROOT_DIR}/demo ${CMAKE_CURRENT_BINARY_DIR}/demo)

list(APPEND CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})

#add_dependencies(context-lib-static context-deps)
add_dependencies(context-demo context-lib-static)

add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")

if (UNIX)
    set(EXTERNAL_DLL_LOCATION ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib)
    file(GLOB EXTERNAL_DLL_GLOB ${EXTERNAL_DLL_LOCATION}/*.so*)
elseif (WIN32)
    set(EXTERNAL_DLL_LOCATION ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin)
    file(GLOB EXTERNAL_DLL_GLOB ${EXTERNAL_DLL_LOCATION}/*dll)
endif ()

add_custom_target(context-workspace
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONTEXT_DEPLOY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/demo
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EXTERNAL_DLL_GLOB} ${CMAKE_CURRENT_BINARY_DIR}/demo
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Copying files to development workspace..."
                  VERBATIM
                  )

set(CONTEXT_PROGRAMS_DIR ${CMAKE_SOURCE_DIR}/programs)
set(CONTEXT_SCENES_DIR ${CMAKE_SOURCE_DIR}/scenes)
set(CONTEXT_GUI_DIR ${CMAKE_SOURCE_DIR}/gui)
set(CONTEXT_TEMP_DIR ${CMAKE_SOURCE_DIR}/tmp)

macro(subdirlist result curdir)
    file(GLOB children RELATIVE ${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${curdir}/${child})
            list(APPEND dirlist ${child})
        endif ()
    endforeach ()
    set(${result} ${dirlist})
endmacro()

macro(zipdirlist curdir destination)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_TEMP_DIR}/${destination})
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        set(filelist "")
        if (IS_DIRECTORY ${curdir}/${child})
            file(GLOB children2 RELATIVE ${curdir}/${child} ${curdir}/${child}/*)
            foreach (child2 ${children2})
                if (NOT IS_DIRECTORY ${curdir}/${child}/${child2})
                    list(APPEND filelist ${child2})
                endif ()
            endforeach ()
            list(APPEND dirlist ${child})
            execute_process(COMMAND ${CMAKE_COMMAND} -E chdir "${curdir}/${child}/" ${CMAKE_COMMAND} -E tar "cf" "${CONTEXT_TEMP_DIR}/${destination}/${child}.zip" --format=zip ${filelist})
        endif ()
    endforeach ()
endmacro()

macro(copydirlist curdir destination)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_TEMP_DIR}/${destination})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${curdir} ${CONTEXT_TEMP_DIR}/${destination})
endmacro()

#copydirlist(${CONTEXT_PROGRAMS_DIR} programs)
#zipdirlist(${CONTEXT_GUI_DIR} gui)
#copydirlist(${CONTEXT_SCENES_DIR} scenes)
zipdirlist(${CONTEXT_PROGRAMS_DIR} programs)
zipdirlist(${CONTEXT_GUI_DIR} gui)
zipdirlist(${CONTEXT_SCENES_DIR} scenes)

install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.json ${EXTERNAL_DLL_GLOB} ${OTHER_DLL_GLOB}
        CONFIGURATIONS Release
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )
file(GLOB CONTEXT_MEDIA_FILES ${CMAKE_SOURCE_DIR}/tmp/*)
install(DIRECTORY ${CONTEXT_MEDIA_FILES}
        CONFIGURATIONS Release
        DESTINATION .
        )
if (UNIX AND NOT MINGW)
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/start.sh
            CONFIGURATIONS Release
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ
            DESTINATION .
            )
endif ()

set(CONTEXT_PACKAGE_NAME "ContextProjectDemo")
set(CONTEXT_ARTIFACT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/artifacts)
set(CONTEXT_ARTIFACT_NAME ${CONTEXT_PACKAGE_NAME}-${CMAKE_BUILD_TYPE}-${CONTEXT_OS}-${CONTEXT_BUILD_PLATFORM})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${CONTEXT_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})

set(CPACK_NSIS_DISPLAY_NAME "Context Demo")
set(CPACK_NSIS_PACKAGE_NAME "Context Demo")
set(CPACK_NSIS_MENU_LINKS "context-demo.exe" "ContextProjectDemo")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL true)

if (UNIX)
    set(CPACK_GENERATOR "TGZ" "7Z")
elseif (WIN32 OR MINGW)
    set(CPACK_GENERATOR "ZIP" "NSIS" "7Z")
endif ()

include(CPack)

add_custom_target(context-package
                  COMMAND ${CMAKE_COMMAND} --build . --target package
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
                  COMMENT "Creating packages..."
                  VERBATIM
                  )

add_custom_target(context-install
                  COMMAND ${CMAKE_COMMAND} --build . --target install
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
                  COMMENT "Installing..."
                  VERBATIM
                  )

add_custom_target(context-install-zip
                  COMMAND zip -r ${CONTEXT_ZIP_NAME}.zip ${CONTEXT_ZIP_NAME}
                  WORKING_DIRECTORY ${CONTEXT_INSTALL_PATH}
                  COMMENT "Create zip..."
                  VERBATIM
                  )

add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)
# include guard
if (_cppflags_included)
    return()
endif (_cppflags_included)
set(_cppflags_included true)

include(CMakeDependentOption)
include(ProcessorCount)

processorcount(PROCESSOR_COUNT)
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(ENABLE_FASTMATH ON)
set(ENABLE_AVX ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC))
    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument")
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
        endif ()
    elseif (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
        string(APPEND CMAKE_CXX_FLAGS " -D_DEBUG -DDEBUG")
    elseif (${CMAKE_BUILD_TYPE_LOWER} MATCHES "deb")
        string(APPEND CMAKE_CXX_FLAGS " -DDEBUG")
    endif ()

    if (ENABLE_FASTMATH)
        string(APPEND CMAKE_CXX_FLAGS " -ffast-math")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -funroll-loops")

    if (WIN32)
        string(APPEND CMAKE_CXX_FLAGS " -DVC_EXTRALEAN -DWIN32_LEAN_AND_MEAN")
    endif ()

    # avoid AppleClang
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-ld=lld")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -pthread")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -pthread")

    if (UNIX)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "X86")
            string(APPEND CMAKE_CXX_FLAGS " -msse4")
        endif ()
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
            if (ENABLE_AVX)
                string(APPEND CMAKE_CXX_FLAGS " -mavx2")
            endif ()
        endif ()
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
    endif ()

    # fix for clang futures issue
    # https://github.com/msys2/MINGW-packages/issues/5786#issuecomment-537495579
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -femulated-tls")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -static-libstdc++")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

elseif (MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /MP /GS- /GF /source-charset:utf-8 /execution-charset:utf-8 /DVC_EXTRALEAN /DWIN32_LEAN_AND_MEAN")

    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "relwithdebinfo")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /SUBSYSTEM:CONSOLE")
    endif ()

    if (${CMAKE_BUILD_TYPE_LOWER} MATCHES "deb")
        string(APPEND CMAKE_CXX_FLAGS " /DDEBUG")
    endif()

    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " /Ot /GL")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    endif ()

    if (ENABLE_FASTMATH)
        string(APPEND CMAKE_CXX_FLAGS " /fp:fast")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "X86")
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        if (ENABLE_AVX)
            string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
        endif ()
        string(REPLACE "/arch:SSE2" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif ()

else ()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} is not supported")
endif ()

# include guard
if (_cppflags_included)
    return()
endif (_cppflags_included)
set(_cppflags_included true)


include(CMakeDependentOption)
include(ProcessorCount)

processorcount(PROCESSOR_COUNT)
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)


option(ENABLE_SSE "ENABLE_SSE" ON)
option(ENABLE_AVX2 "ENABLE_AVX2" ON)
option(ENABLE_FASTMATH "ENABLE_FASTMATH" ON)
option(ENABLE_UNROLL_LOOPS "ENABLE_UNROLL_LOOPS" ON)
option(DISABLE_EXCEPTIONS "DISABLE_EXCEPTIONS" OFF)
option(DISABLE_RTTI "DISABLE_RTTI" OFF)
option(LTO "ENABLE LTO" ON)
cmake_dependent_option(ENABLE_CXX_STATIC "ENABLE_CXX_STATIC" ON "CMAKE_CXX_COMPILER_ID STREQUAL GNU AND NOT MINGW" OFF)


if ((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND NOT MSVC)
    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument")
        string(APPEND CMAKE_C_FLAGS " -g0 -s -Wno-unused-command-line-argument")
        if (LTO)
            if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
                string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
            elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
                string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
            endif ()
        endif ()
    elseif (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
        string(APPEND CMAKE_CXX_FLAGS " -D_DEBUG -DDEBUG")
        string(APPEND CMAKE_C_FLAGS " -D_DEBUG -DDEBUG")
    elseif (${CMAKE_BUILD_TYPE_LOWER} MATCHES "deb")
        string(APPEND CMAKE_CXX_FLAGS " -DDEBUG")
        string(APPEND CMAKE_C_FLAGS " -DDEBUG")
    endif ()

    if (ENABLE_FASTMATH)
        string(APPEND CMAKE_CXX_FLAGS " -ffast-math")
        string(APPEND CMAKE_C_FLAGS " -ffast-math")
    endif ()

    if (ENABLE_UNROLL_LOOPS)
        string(APPEND CMAKE_CXX_FLAGS " -funroll-loops")
        string(APPEND CMAKE_C_FLAGS " -funroll-loops")
    endif ()

    if (DISABLE_EXCEPTIONS)
        string(APPEND CMAKE_CXX_FLAGS " -D_HAS_EXCEPTIONS=0 -fno-exceptions")
    endif ()

    if (DISABLE_RTTI)
        string(APPEND CMAKE_CXX_FLAGS " -fno-rtti")
    endif ()

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

    if (EMSCRIPTEN)
        string(APPEND CMAKE_CXX_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0")
        string(APPEND CMAKE_C_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ALLOW_MEMORY_GROWTH=1 -s PTHREAD_POOL_SIZE=32 -s FULL_ES3=1 -s USE_ZLIB=1 -s USE_SDL=2")
        if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASSERTIONS=1")
        endif ()
    endif ()

    if (UNIX)
        if (ENABLE_SSE AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "X86"))
            string(APPEND CMAKE_CXX_FLAGS " -msse4")
            string(APPEND CMAKE_C_FLAGS " -msse4")
        endif ()
        if (ENABLE_AVX2 AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
            string(APPEND CMAKE_CXX_FLAGS " -mavx2")
            string(APPEND CMAKE_C_FLAGS " -mavx2")
        endif ()
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    # fix for clang futures issue
    # https://github.com/msys2/MINGW-packages/issues/5786#issuecomment-537495579
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -femulated-tls")
        string(APPEND CMAKE_C_FLAGS " -femulated-tls")
    endif ()

    if (ENABLE_CXX_STATIC)
        string(APPEND CMAKE_CXX_FLAGS " -static-libstdc++")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

elseif (MSVC)
    if (DISABLE_EXCEPTIONS)
        string(APPEND CMAKE_CXX_FLAGS " /D_HAS_EXCEPTIONS=0")
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif ()

    if (DISABLE_RTTI)
        string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " /MP /GS- /GF /utf-8 /DVC_EXTRALEAN /DWIN32_LEAN_AND_MEAN")
    string(APPEND CMAKE_C_FLAGS " /MP /GS- /GF /utf-8")

    if (${CMAKE_BUILD_TYPE_LOWER} MATCHES "deb")
        string(APPEND CMAKE_CXX_FLAGS " /DDEBUG")
        string(APPEND CMAKE_C_FLAGS " /DDEBUG")
    endif()

    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " /Ot /GL")
        string(APPEND CMAKE_C_FLAGS " /Ot /GL")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    endif ()

    if (ENABLE_FASTMATH)
        string(APPEND CMAKE_CXX_FLAGS " /fp:fast")
        string(APPEND CMAKE_C_FLAGS " /fp:fast")
    endif ()

    if (ENABLE_SSE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "X86")
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
        string(APPEND CMAKE_C_FLAGS " /arch:SSE2")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        if (ENABLE_AVX2)
            string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
            string(APPEND CMAKE_C_FLAGS " /arch:AVX2")
        endif ()
        if (ENABLE_SSE)
            string(REPLACE "/arch:SSE2" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
            string(REPLACE "/arch:SSE2" "" CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
        endif ()
    endif ()

else ()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} is not known")
endif ()

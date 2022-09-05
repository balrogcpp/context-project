# This file is part of Glue Engine. Created by Andrey Vasiliev
# C++ optimization flags


include(ProcessorCount)
processorcount(PROCESSOR_COUNT)


set(USE_AVX2 OFF)
set(USE_FASTMATH ON)


if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC) OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
    string(REPLACE " -DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    string(REPLACE " -DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -g0 -s -Wno-unused-command-line-argument -funroll-loops -DNDEBUG")
        if (USE_FASTMATH)
            string(APPEND CMAKE_CXX_FLAGS " -ffast-math")
            string(APPEND CMAKE_C_FLAGS " -ffast-math")
        endif ()
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
        else ()
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
        endif ()
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        string(APPEND CMAKE_CXX_FLAGS " -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -DDEBUG -D_DEBUG")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld ")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-ld=lld ")
    endif ()

    string(APPEND CMAKE_EXE_LINKER_FLAGS " -pthread")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -pthread")

    if (EMSCRIPTEN)
        string(APPEND CMAKE_CXX_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
        string(APPEND CMAKE_C_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        string(APPEND CMAKE_CXX_FLAGS " -msse4")
        string(APPEND CMAKE_C_FLAGS " -msse4")
    endif ()

    if (USE_AVX2 AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        string(APPEND CMAKE_CXX_FLAGS " -mavx2")
        string(APPEND CMAKE_C_FLAGS " -mavx2")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

elseif (MSVC)
    string(REPLACE " /DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    string(REPLACE " /DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    string(APPEND CMAKE_CXX_FLAGS " /EHa /MP /GS- /GF /utf-8")
    string(APPEND CMAKE_C_FLAGS " /EHa /MP /GS- /GF /utf-8")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /Ot /Ob2 /GL /DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " /O2 /Ot /Ob2 /GL /DNDEBUG")
        if (USE_FASTMATH)
            string(APPEND CMAKE_CXX_FLAGS " /fp:fast")
            string(APPEND CMAKE_C_FLAGS " /fp:fast")
        endif ()
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /Zi /Ob1 /DDEBUG")
        string(APPEND CMAKE_C_FLAGS " /Zi /Ob1 /DDEBUG")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "X86" AND CMAKE_SIZEOF_VOID_P LESS 8)
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
        string(APPEND CMAKE_C_FLAGS " /arch:SSE2")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" AND USE_AVX2)
        string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
        string(APPEND CMAKE_C_FLAGS " /arch:AVX2")
    endif ()
endif ()


# These variables are used for dependencies
set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXTRA_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_EXTRA_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
set(CMAKE_EXTRA_STATIC_LINKER_FLAGS ${CMAKE_STATIC_LINKER_FLAGS})

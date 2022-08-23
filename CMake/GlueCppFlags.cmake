# This file is part of Glue Engine. Created by Andrey Vasiliev
# C++ optimization flags


include(ProcessorCount)
processorcount(PROCESSOR_COUNT)


option(GLUE_USE_AVX "Use AVX instructions set" OFF)
option(GLUE_USE_AVX2 "Use AVX2 instructions set" OFF)


if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT MSVC)
    string(APPEND CMAKE_CXX_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wall")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument -O3 -ffast-math -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -g0 -s -Wno-unused-command-line-argument -O3 -ffast-math -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-linker-plugin")
        if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
        else ()
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
        endif ()
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    endif ()
    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    string(APPEND CMAKE_C_FLAGS " -pthread")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -pthread")

    if (EMSCRIPTEN)
        string(APPEND CMAKE_CXX_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
        string(APPEND CMAKE_C_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s PTHREAD_POOL_SIZE=4 -s SHARED_MEMORY=1")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        string(APPEND CMAKE_CXX_FLAGS " -msse4")
        string(APPEND CMAKE_C_FLAGS " -msse4")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(APPEND CMAKE_CXX_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_C_FLAGS " -floop-parallelize-all")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    if (GLUE_USE_AVX2)
        string(APPEND CMAKE_CXX_FLAGS " -mavx2")
        string(APPEND CMAKE_C_FLAGS " -mavx2")
    elseif (GLUE_USE_AVX)
        string(APPEND CMAKE_CXX_FLAGS " -mavx")
        string(APPEND CMAKE_C_FLAGS " -mavx")
    endif ()

    # gcc-mingw links everything as shared libraries by default
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

    option(GLUE_USE_OPENMP "" OFF)
    if (GLUE_USE_OPENMP)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(APPEND CMAKE_C_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
            string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -fopenmp -static-openmp")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fopenmp -static-openmp")
        endif ()
    endif ()
elseif (MSVC)
    string(REPLACE " /DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    string(REPLACE " /DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    if (CMAKE_SIZEOF_VOID_P LESS 8)
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
        string(APPEND CMAKE_C_FLAGS " /arch:SSE2")
    endif ()

    if (GLUE_USE_AVX2)
        string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
        string(APPEND CMAKE_C_FLAGS " /arch:AVX2")
    elseif (GLUE_USE_AVX)
        string(APPEND CMAKE_CXX_FLAGS " /arch:AVX")
        string(APPEND CMAKE_C_FLAGS " /arch:AVX")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " /EHa /MP /GS- /GF /utf-8")
    string(APPEND CMAKE_C_FLAGS " /EHa /MP /GS- /GF /utf-8")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL /DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL /DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /Zi /Ob1 /DDEBUG")
        string(APPEND CMAKE_C_FLAGS " /Zi /Ob1 /DDEBUG")
    endif ()
endif ()


# This variables are used for dependencies
set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXTRA_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_EXTRA_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
set(CMAKE_EXTRA_STATIC_LINKER_FLAGS ${CMAKE_STATIC_LINKER_FLAGS})

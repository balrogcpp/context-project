# This source file is part of "glue project". Created by Andrey Vasiliev

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_CXX_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wall")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument -Ofast -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -g0 -s -Wno-unused-command-line-argument -Ofast -funroll-loops -DNDEBUG")
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
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        string(APPEND CMAKE_CXX_FLAGS " -msse4.1")
        string(APPEND CMAKE_C_FLAGS " -msse4.1")
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(APPEND CMAKE_CXX_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_C_FLAGS " -floop-parallelize-all")
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    # gcc-mingw links everything as shared libraries by default
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    string(APPEND CMAKE_C_FLAGS " -pthread")

    option(GLUE_ENABLE_OPENMP "" OFF)
    if (GLUE_ENABLE_OPENMP)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(APPEND CMAKE_C_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
            string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -fopenmp -static-openmp")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fopenmp -static-openmp")
        endif ()
    endif ()
elseif (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    if (CMAKE_SIZEOF_VOID_P LESS 8)
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
        string(APPEND CMAKE_C_FLAGS " /arch:SSE2")
    endif ()
    string(APPEND CMAKE_CXX_FLAGS " /EHa /MP /GS- /GF")
    string(APPEND CMAKE_C_FLAGS " /EHa /MP /GS- /GF")
    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL")
        string(APPEND CMAKE_C_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_C_FLAGS " /D \"DEBUG\"")
    endif ()
endif ()

# This flags are useful for thirdparty build
set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS})

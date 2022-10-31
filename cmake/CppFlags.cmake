# include guard
if (_cppflags_included)
    return()
endif (_cppflags_included)
set(_cppflags_included true)


include(ProcessorCount)
processorcount(PROCESSOR_COUNT)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(USE_AVX2 OFF)
set(USE_FASTMATH ON)
set(USE_EXCEPTIONS ON)
set(USE_RTTI ON)


if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC) OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
#    string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
#    string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " -g0 -s -Wno-unused-command-line-argument -funroll-loops")
        string(APPEND CMAKE_C_FLAGS " -g0 -s -Wno-unused-command-line-argument -funroll-loops")
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
    elseif (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug" OR ${CMAKE_BUILD_TYPE_LOWER} STREQUAL "relwithdebinfo")
        string(APPEND CMAKE_CXX_FLAGS " -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -D_DEBUG")
    endif ()

    if (NOT USE_EXCEPTIONS)
        string(APPEND CMAKE_CXX_FLAGS " -D_HAS_EXCEPTIONS=0")
        string(APPEND CMAKE_CXX_FLAGS " -fno-exceptions")
    endif()

    if (NOT USE_RTTI)
        string(APPEND CMAKE_CXX_FLAGS " -fno-rtti")
    endif()


    if (WIN32)
        string(APPEND CMAKE_CXX_FLAGS " -DVC_EXTRALEAN -DWIN32_LEAN_AND_MEAN")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-ld=lld")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    string(APPEND CMAKE_C_FLAGS " -pthread")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -pthread")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -pthread")

    if (EMSCRIPTEN)
        string(APPEND CMAKE_CXX_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0")
        string(APPEND CMAKE_C_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ALLOW_MEMORY_GROWTH=1 -s PTHREAD_POOL_SIZE=4 -s FULL_ES3=1 -s USE_ZLIB=1 -s USE_SDL=2")
        if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASSERTIONS=1")
        endif ()
    endif ()

    if (UNIX)
        if (NOT USE_AVX2 AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "X86"))
            string(APPEND CMAKE_CXX_FLAGS " -msse4")
            string(APPEND CMAKE_C_FLAGS " -msse4")
        elseif (USE_AVX2 AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
            string(APPEND CMAKE_CXX_FLAGS " -mavx2")
            string(APPEND CMAKE_C_FLAGS " -mavx2")
        endif ()
    elseif(WIN32)
        if (NOT USE_AVX2 AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "X86"))
            string(APPEND CMAKE_CXX_FLAGS " -msse4")
            string(APPEND CMAKE_C_FLAGS " -msse4")
        elseif (USE_AVX2 AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
            string(APPEND CMAKE_CXX_FLAGS " -mavx2")
            string(APPEND CMAKE_C_FLAGS " -mavx2")
        endif ()
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

elseif (MSVC)
#    string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
#    string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    if (NOT USE_EXCEPTIONS)
        string(APPEND CMAKE_CXX_FLAGS " /D_HAS_EXCEPTIONS=0")
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif()

    if (NOT USE_RTTI)
        string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif()

    string(APPEND CMAKE_CXX_FLAGS " /MP /GS- /GF /utf-8 /D_USE_MATH_DEFINES /DVC_EXTRALEAN /DWIN32_LEAN_AND_MEAN")
    string(APPEND CMAKE_C_FLAGS " /MP /GS- /GF /utf-8 /D_USE_MATH_DEFINES")

    if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "release")
        string(APPEND CMAKE_CXX_FLAGS " /Ot /GL")
        string(APPEND CMAKE_C_FLAGS " /Ot /GL")
        if (USE_FASTMATH)
            string(APPEND CMAKE_CXX_FLAGS " /fp:fast")
            string(APPEND CMAKE_C_FLAGS " /fp:fast")
        endif ()
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    endif ()

    if (USE_AVX2 AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
        string(APPEND CMAKE_C_FLAGS " /arch:AVX2")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "X86")
        string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2")
        string(APPEND CMAKE_C_FLAGS " /arch:SSE2")
    endif ()
endif ()

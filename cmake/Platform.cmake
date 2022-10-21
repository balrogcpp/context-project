# include guard
if (_platform_included)
    return()
endif (_platform_included)
set(_platform_included true)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(TOOLCHAIN_SHORT "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
set(TOOLCHAIN_FULL "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_CXX_COMPILER_ID}")
if (MINGW)
    string(APPEND TOOLCHAIN_FULL "_Mingw")
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX 1)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(MACOS 1)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS 1)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DESKTOP 1)
elseif (ANDROID OR IOS)
    set(MOBILE 1)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
    set(GCC 1)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CLANG 1)
endif ()

if (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set(XCODE 1)
endif ()

if (CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    set(DEBUG 1)
elseif (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    set(RELEASE 1)
elseif (CMAKE_BUILD_TYPE_LOWER STREQUAL "relwithdebinfo")
    set(RELWITHDEBINFO 1)
endif ()

if (MOBILE OR EMSCRIPTEN)
    set(GLSLES 1)
else ()
    set(GLSL 1)
endif ()

# include guard
if (_platform_included)
    return()
endif (_platform_included)
set(_platform_included true)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GCC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # to cover AppleClang
    set(CLANG 1)
endif ()

if ((MSVC AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64") OR (UNIX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"))
    set(CMAKE_SYSTEM_PROCESSOR_NORMALISED "amd64")
else ()
    set(CMAKE_SYSTEM_PROCESSOR_NORMALISED ${CMAKE_SYSTEM_PROCESSOR})
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_SYSTEM_NAME_NORMALISED "Win32")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_SYSTEM_NAME_NORMALISED "MacOS")
else ()
    set(CMAKE_SYSTEM_NAME_NORMALISED ${CMAKE_SYSTEM_NAME})
endif ()


string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
set(TOOLCHAIN_SHORT_NORMALISED "${CMAKE_SYSTEM_NAME_NORMALISED}-${CMAKE_SYSTEM_PROCESSOR_NORMALISED}")
set(TOOLCHAIN_FULL_NORMALISED "${CMAKE_SYSTEM_NAME_NORMALISED}-${CMAKE_SYSTEM_PROCESSOR_NORMALISED}-${CMAKE_CXX_COMPILER_ID}")
if (MINGW)
    string(APPEND TOOLCHAIN_FULL_NORMALISED "-MinGW")
elseif (MSVC AND CLANG)
    string(APPEND TOOLCHAIN_FULL_NORMALISED "-MSVC")
endif ()
string(TOLOWER ${TOOLCHAIN_FULL_NORMALISED} TOOLCHAIN_FULL_NORMALISED)
string(TOLOWER ${TOOLCHAIN_SHORT_NORMALISED} TOOLCHAIN_SHORT_NORMALISED)

set(TOOLCHAIN_SHORT "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
set(TOOLCHAIN_FULL "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_CXX_COMPILER_ID}")
if (MINGW)
    string(APPEND TOOLCHAIN_FULL "_MinGW")
elseif (MSVC AND CLANG)
    string(APPEND TOOLCHAIN_FULL "_MSVC")
endif ()
string(TOLOWER ${TOOLCHAIN_FULL} TOOLCHAIN_FULL_LOWER)
string(TOLOWER ${TOOLCHAIN_SHORT} TOOLCHAIN_SHORT_LOWER)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX 1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(MACOS 1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS 1)
endif ()


if (LINUX OR MACOS OR WINDOWS)
    set(DESKTOP 1)
elseif (ANDROID OR IOS)
    set(MOBILE 1)
endif ()



if (CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    set(DEBUG 1)
    set(CMAKE_BUILD_TYPE_NORMALISED "Debug")
elseif (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    set(RELEASE 1)
    set(CMAKE_BUILD_TYPE_NORMALISED "Release")
elseif (CMAKE_BUILD_TYPE_LOWER STREQUAL "relwithdebinfo")
    set(RELWITHDEBINFO 1)
    set(CMAKE_BUILD_TYPE_NORMALISED "RelWithDebInfo")
endif ()


if (MOBILE OR EMSCRIPTEN)
    set(GLSLES 1)
else ()
    set(GLSL 1)
endif ()

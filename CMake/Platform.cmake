# include guard
if (_platform_included)
    return()
endif (_platform_included)
set(_platform_included true)


if (MSVC)
    set(TOOLCHAIN_FULL "${CMAKE_SYSTEM_NAME}-${CMAKE_VS_PLATFORM_NAME}-${CMAKE_CXX_COMPILER_ID}")
else ()
    set(TOOLCHAIN_FULL "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
endif ()

string(TOLOWER ${TOOLCHAIN_FULL} TOOLCHAIN_FULL)
message(${TOOLCHAIN_FULL})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GCC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # to cover AppleClang
    set(CLANG 1)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX 1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OSX 1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS 1)
endif ()

if (LINUX OR OSX OR WINDOWS)
    set(DESKTOP 1)
elseif (ANDROID OR IOS)
    set(MOBILE 1)
endif ()

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    set(DEBUG 1)
elseif (CMAKE_BUILD_TYPE_LOWER MATCHES "release")
    set(RELEASE 1)
elseif (CMAKE_BUILD_TYPE_LOWER MATCHES "relwithdebinfo")
    set(RELWITHDEBINFO 1)
endif ()

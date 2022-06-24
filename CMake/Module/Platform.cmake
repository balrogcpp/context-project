# This file is part of Glue Engine. Created by Andrey Vasiliev
# Define default platforms

set(GLUE_TOOLCHAIN_SHORT "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
set(GLUE_TOOLCHAIN_FULL "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_CXX_COMPILER_ID}")
if (MINGW)
    string(APPEND GLUE_TOOLCHAIN_FULL "_Mingw")
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

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG 1)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RELEASE 1)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(RELWITHDEBINFO 1)
endif ()

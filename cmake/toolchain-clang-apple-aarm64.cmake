# OSXCross toolchain

set(ENV{OSXCROSS_HOST} aarch64-apple)
execute_process(COMMAND osxcross-conf OUTPUT_VARIABLE OSXCROSS_ENV OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "export " "" OSXCROSS_ENV ${OSXCROSS_ENV})
string(REPLACE "\n" ";" OSXCROSS_ENV ${OSXCROSS_ENV})

foreach (VAR ${OSXCROSS_ENV})
    cmake_policy(PUSH)
    cmake_policy(SET CMP0007 NEW)
    string(REPLACE "=" ";" VAR ${VAR})
    list(GET VAR 0 name)
    list(GET VAR 1 value)
    if (name AND value)
        set(ENV{${name}} ${value})
    endif ()
    cmake_policy(POP)
endforeach ()

macro(osxcross_getconf VAR)
    if (NOT ${VAR})
        set(${VAR} "$ENV{${VAR}}")
        if (${VAR})
            set(${VAR} "${${VAR}}" CACHE STRING "${VAR}")
            message(STATUS "Found ${VAR}: ${${VAR}}")
        else ()
            message(FATAL_ERROR "Cannot determine \"${VAR}\"")
        endif ()
    endif ()
endmacro()

osxcross_getconf(OSXCROSS_HOST)
osxcross_getconf(OSXCROSS_TARGET_DIR)
osxcross_getconf(OSXCROSS_TARGET)
osxcross_getconf(OSXCROSS_SDK)

set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_SYSTEM_NAME "Darwin")
string(REGEX REPLACE "-.*" "" CMAKE_SYSTEM_PROCESSOR "${OSXCROSS_HOST}-${OSXCROSS_TARGET}")

# specify the cross compiler
set(CMAKE_C_COMPILER "${OSXCROSS_TARGET_DIR}/bin/${OSXCROSS_HOST}-${OSXCROSS_TARGET}-clang")
set(CMAKE_CXX_COMPILER "${OSXCROSS_TARGET_DIR}/bin/${OSXCROSS_HOST}-${OSXCROSS_TARGET}-clang++")

# where is the target environment
set(CMAKE_FIND_ROOT_PATH
        "${CMAKE_FIND_ROOT_PATH}"
        "${OSXCROSS_SDK}"
        "${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_AR "${OSXCROSS_TARGET_DIR}/bin/${OSXCROSS_HOST}-${OSXCROSS_TARGET}-ar" CACHE FILEPATH "ar")
set(CMAKE_RANLIB "${OSXCROSS_TARGET_DIR}/bin/${OSXCROSS_HOST}-${OSXCROSS_TARGET}-ranlib" CACHE FILEPATH "ranlib")
set(CMAKE_INSTALL_NAME_TOOL "${OSXCROSS_TARGET_DIR}/bin/${OSXCROSS_HOST}-${OSXCROSS_TARGET}-install_name_tool" CACHE FILEPATH "install_name_tool")

set(ENV{PKG_CONFIG_LIBDIR} "${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/lib/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${OSXCROSS_TARGET_DIR}/macports/pkgs")

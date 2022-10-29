project(lua C)
cmake_minimum_required(VERSION 2.9)
find_package(Git REQUIRED)

# installation directories
set(INSTALL_BIN bin CACHE PATH "Where to install binaries to.")
set(INSTALL_LIB lib CACHE PATH "Where to install libraries to.")
set(INSTALL_INC include CACHE PATH "Where to install headers to.")
set(INSTALL_ETC etc CACHE PATH "Where to store configuration files")
set(INSTALL_SHARE share CACHE PATH "Directory for shared data.")

# Secondary paths
option(INSTALL_VERSION "Install runtime libraries and executables with version information." OFF)
set(INSTALL_DATA ${INSTALL_SHARE}/${DIST_NAME} CACHE PATH "Directory the package can store documentation, tests or other data in.")
set(INSTALL_DOC ${INSTALL_DATA}/doc CACHE PATH "Recommended directory to install documentation into.")
set(INSTALL_EXAMPLE ${INSTALL_DATA}/example CACHE PATH "Recommended directory to install examples into.")
set(INSTALL_TEST ${INSTALL_DATA}/test CACHE PATH "Recommended directory to install tests into.")
set(INSTALL_FOO ${INSTALL_DATA}/etc CACHE PATH "Where to install additional files")

# Tweaks and other defaults
# Setting CMAKE to use loose block and search for find modules in source directory
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# In MSVC, prevent warnings that can occur when using standard libraries.
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

# RPath and relative linking
option(USE_RPATH "Use relative linking." ON)
if (USE_RPATH)
    string(REGEX REPLACE "[^!/]+" ".." UP_DIR ${INSTALL_BIN})
    set(CMAKE_SKIP_BUILD_RPATH FALSE CACHE STRING "" FORCE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE CACHE STRING "" FORCE)
    set(CMAKE_INSTALL_RPATH $ORIGIN/${UP_DIR}/${INSTALL_LIB}
            CACHE STRING "" FORCE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE)
    set(CMAKE_INSTALL_NAME_DIR @executable_path/${UP_DIR}/${INSTALL_LIB}
            CACHE STRING "" FORCE)
endif ()

## MACROS
# Parser macro
macro(parse_arguments prefix arg_names option_names)
    set(DEFAULT_ARGS)
    foreach (arg_name ${arg_names})
        set(${prefix}_${arg_name})
    endforeach ()
    foreach (option ${option_names})
        set(${prefix}_${option} FALSE)
    endforeach ()

    set(current_arg_name DEFAULT_ARGS)
    set(current_arg_list)
    foreach (arg ${ARGN})
        set(larg_names ${arg_names})
        list(FIND larg_names "${arg}" is_arg_name)
        if (is_arg_name GREATER -1)
            set(${prefix}_${current_arg_name} ${current_arg_list})
            set(current_arg_name ${arg})
            set(current_arg_list)
        else ()
            set(loption_names ${option_names})
            list(FIND loption_names "${arg}" is_option)
            if (is_option GREATER -1)
                set(${prefix}_${arg} TRUE)
            else ()
                set(current_arg_list ${current_arg_list} ${arg})
            endif ()
        endif ()
    endforeach ()
    set(${prefix}_${current_arg_name} ${current_arg_list})
endmacro()


# install_executable ( executable_targets )
# Installs any executables generated using "add_executable".
# USE: install_executable ( lua )
# NOTE: subdirectories are NOT supported
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "${DIST_NAME} Runtime")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION
        "Executables and runtime libraries. Installed into ${INSTALL_BIN}.")
macro(install_executable)
    foreach (_file ${ARGN})
        if (INSTALL_VERSION)
            set_target_properties(${_file} PROPERTIES VERSION ${DIST_VERSION}
                    SOVERSION ${DIST_VERSION})
        endif ()
        install(TARGETS ${_file} RUNTIME DESTINATION ${INSTALL_BIN}
                COMPONENT Runtime)
    endforeach ()
endmacro()

# install_library ( library_targets )
# Installs any libraries generated using "add_library" into apropriate places.
# USE: install_library ( libexpat )
# NOTE: subdirectories are NOT supported
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "${DIST_NAME} Development Libraries")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION
        "Static and import libraries needed for development. Installed into ${INSTALL_LIB} or ${INSTALL_BIN}.")
macro(install_library)
    foreach (_file ${ARGN})
        if (INSTALL_VERSION)
            set_target_properties(${_file} PROPERTIES VERSION ${DIST_VERSION}
                    SOVERSION ${DIST_VERSION})
        endif ()
        install(TARGETS ${_file}
                RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime
                LIBRARY DESTINATION ${INSTALL_LIB} COMPONENT Runtime
                ARCHIVE DESTINATION ${INSTALL_LIB} COMPONENT Library)
    endforeach ()
endmacro()

# helper function for various install_* functions, for PATTERN/REGEX args.
macro(_complete_install_args)
    if (NOT ("${_ARG_PATTERN}" STREQUAL ""))
        set(_ARG_PATTERN PATTERN ${_ARG_PATTERN})
    endif ()
    if (NOT ("${_ARG_REGEX}" STREQUAL ""))
        set(_ARG_REGEX REGEX ${_ARG_REGEX})
    endif ()
endmacro()

# install_header ( files/directories [INTO destination] )
# Install a directories or files into header destination.
# USE: install_header ( lua.h luaconf.h ) or install_header ( GL )
# USE: install_header ( mylib.h INTO mylib )
# For directories, supports optional PATTERN/REGEX arguments like install().
set(CPACK_COMPONENT_HEADER_DISPLAY_NAME "${DIST_NAME} Development Headers")
set(CPACK_COMPONENT_HEADER_DESCRIPTION
        "Headers needed for development. Installed into ${INSTALL_INC}.")
macro(install_header)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file} DESTINATION ${INSTALL_INC}/${_ARG_INTO}
                    COMPONENT Header ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_INC}/${_ARG_INTO}
                    COMPONENT Header)
        endif ()
    endforeach ()
endmacro()

# install_data ( files/directories [INTO destination] )
# This installs additional data files or directories.
# USE: install_data ( extra data.dat )
# USE: install_data ( image1.png image2.png INTO images )
# For directories, supports optional PATTERN/REGEX arguments like install().
set(CPACK_COMPONENT_DATA_DISPLAY_NAME "${DIST_NAME} Data")
set(CPACK_COMPONENT_DATA_DESCRIPTION
        "Application data. Installed into ${INSTALL_DATA}.")
macro(install_data)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file}
                    DESTINATION ${INSTALL_DATA}/${_ARG_INTO}
                    COMPONENT Data ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_DATA}/${_ARG_INTO}
                    COMPONENT Data)
        endif ()
    endforeach ()
endmacro()

# INSTALL_DOC ( files/directories [INTO destination] )
# This installs documentation content
# USE: install_doc ( doc/ doc.pdf )
# USE: install_doc ( index.html INTO html )
# For directories, supports optional PATTERN/REGEX arguments like install().
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "${DIST_NAME} Documentation")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION
        "Application documentation. Installed into ${INSTALL_DOC}.")
macro(install_doc)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file} DESTINATION ${INSTALL_DOC}/${_ARG_INTO}
                    COMPONENT Documentation ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_DOC}/${_ARG_INTO}
                    COMPONENT Documentation)
        endif ()
    endforeach ()
endmacro()

# install_example ( files/directories [INTO destination]  )
# This installs additional examples
# USE: install_example ( examples/ exampleA )
# USE: install_example ( super_example super_data INTO super)
# For directories, supports optional PATTERN/REGEX argument like install().
set(CPACK_COMPONENT_EXAMPLE_DISPLAY_NAME "${DIST_NAME} Examples")
set(CPACK_COMPONENT_EXAMPLE_DESCRIPTION
        "Examples and their associated data. Installed into ${INSTALL_EXAMPLE}.")
macro(install_example)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file} DESTINATION ${INSTALL_EXAMPLE}/${_ARG_INTO}
                    COMPONENT Example ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_EXAMPLE}/${_ARG_INTO}
                    COMPONENT Example)
        endif ()
    endforeach ()
endmacro()

# install_test ( files/directories [INTO destination] )
# This installs tests and test files, DOES NOT EXECUTE TESTS
# USE: install_test ( my_test data.sql )
# USE: install_test ( feature_x_test INTO x )
# For directories, supports optional PATTERN/REGEX argument like install().
set(CPACK_COMPONENT_TEST_DISPLAY_NAME "${DIST_NAME} Tests")
set(CPACK_COMPONENT_TEST_DESCRIPTION
        "Tests and associated data. Installed into ${INSTALL_TEST}.")
macro(install_test)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file} DESTINATION ${INSTALL_TEST}/${_ARG_INTO}
                    COMPONENT Test ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_TEST}/${_ARG_INTO}
                    COMPONENT Test)
        endif ()
    endforeach ()
endmacro()

# install_foo ( files/directories [INTO destination] )
# This installs optional or otherwise unneeded content
# USE: install_foo ( etc/ example.doc )
# USE: install_foo ( icon.png logo.png INTO icons)
# For directories, supports optional PATTERN/REGEX argument like install().
set(CPACK_COMPONENT_OTHER_DISPLAY_NAME "${DIST_NAME} Unspecified Content")
set(CPACK_COMPONENT_OTHER_DESCRIPTION
        "Other unspecified content. Installed into ${INSTALL_FOO}.")
macro(install_foo)
    parse_arguments(_ARG "INTO;PATTERN;REGEX" "" ${ARGN})
    _complete_install_args()
    foreach (_file ${_ARG_DEFAULT_ARGS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
            install(DIRECTORY ${_file} DESTINATION ${INSTALL_FOO}/${_ARG_INTO}
                    COMPONENT Other ${_ARG_PATTERN} ${_ARG_REGEX})
        else ()
            install(FILES ${_file} DESTINATION ${INSTALL_FOO}/${_ARG_INTO}
                    COMPONENT Other)
        endif ()
    endforeach ()
endmacro()

## CTest defaults

## CPack defaults
set(CPACK_GENERATOR "ZIP")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PACKAGE_NAME "${DIST_NAME}")
set(CPACK_PACKAGE_VERSION "${DIST_VERSION}")
set(CPACK_PACKAGE_VENDOR "LuaDist")
set(CPACK_COMPONENTS_ALL Runtime Library Header Data Documentation Example Other)
include(CPack)


## CONFIGURATION
# Default configuration (we assume POSIX by default)
set(LUA_PATH "LUA_PATH" CACHE STRING "Environment variable to use as package.path.")
set(LUA_CPATH "LUA_CPATH" CACHE STRING "Environment variable to use as package.cpath.")
set(LUA_INIT "LUA_INIT" CACHE STRING "Environment variable for initial script.")
set(LUA_EXE "LUA_EXE" CACHE STRING "Build lua executables.")
set(LUA_VERSION "v5.4.4" CACHE STRING "Lua version to fetch")

option(LUA_USE_C89 "Use only C89 features." OFF)
option(LUA_USE_RELATIVE_LOADLIB "Use modified loadlib.c with support for relative paths on posix systems." ON)

option(LUA_COMPAT_5_1 "Enable backwards compatibility options with lua-5.1." ON)
option(LUA_COMPAT_5_2 "Enable backwards compatibility options with lua-5.2." ON)

#2DO: LUAI_* and LUAL_* settings, for now defaults are used.
set(LUA_DIRSEP "/")
set(LUA_MODULE_SUFFIX ${CMAKE_SHARED_MODULE_SUFFIX})
set(LUA_LDIR ${INSTALL_LMOD})
set(LUA_CDIR ${INSTALL_CMOD})

if (LUA_USE_RELATIVE_LOADLIB)
    # This will set up relative paths to lib
    string(REGEX REPLACE "[^!/]+" ".." LUA_DIR "!/${INSTALL_BIN}/")
else ()
    # Direct path to installation
    set(LUA_DIR ${CMAKE_INSTALL_PREFIX} CACHE STRING "Destination from which modules will be resolved. See INSTALL_LMOD and INSTALL_CMOD.")
endif ()

set(LUA_PATH_DEFAULT "./?.lua;${LUA_DIR}${LUA_LDIR}/?.lua;${LUA_DIR}${LUA_LDIR}/?/init.lua")
set(LUA_CPATH_DEFAULT "./?${LUA_MODULE_SUFFIX};${LUA_DIR}${LUA_CDIR}/?${LUA_MODULE_SUFFIX};${LUA_DIR}${LUA_CDIR}/loadall${LUA_MODULE_SUFFIX}")

if (WIN32 AND NOT CYGWIN)
    # Windows systems
    option(LUA_USE_WINDOWS "Windows specific build." ON)
    option(LUA_BUILD_WLUA "Build wLua interpretter without console output." ON)

    # Paths (Double escapes needed)
    set(LUA_DIRSEP "\\\\")
    string(REPLACE " /" ${LUA_DIRSEP} LUA_DIR "${LUA_DIR}")
    string(REPLACE "/" ${LUA_DIRSEP} LUA_LDIR "${LUA_LDIR}")
    string(REPLACE "/" ${LUA_DIRSEP} LUA_CDIR "${LUA_CDIR}")
    string(REPLACE "/" ${LUA_DIRSEP} LUA_PATH_DEFAULT "${LUA_PATH_DEFAULT}")
    string(REPLACE "/" ${LUA_DIRSEP} LUA_CPATH_DEFAULT "${LUA_CPATH_DEFAULT}")
else ()
    # Posix systems (incl. Cygwin)
    option(LUA_USE_POSIX "Use POSIX features." ON)
    option(LUA_USE_DLOPEN "Use dynamic linker to load modules." ON)
    # Apple and Linux specific
    if (LINUX OR APPLE)
        option(LUA_USE_AFORMAT "Assume 'printf' handles 'aA' specifiers" ON)
    endif ()
endif ()

## SETUP
# Optional libraries
find_package(Readline)
if (READLINE_FOUND)
    option(LUA_USE_READLINE "Use readline in the Lua CLI." ON)
endif ()

# Setup needed variables and libraries
if (LUA_USE_POSIX)
    # On POSIX Lua links to standard math library "m"
    list(APPEND LIBS m)
endif ()

if (LUA_USE_DLOPEN)
    # Link to dynamic linker library "dl"
    find_library(DL_LIBRARY NAMES dl)
    if (DL_LIBRARY)
        list(APPEND LIBS ${DL_LIBRARY})
    endif ()
endif ()

if (LUA_USE_READLINE)
    # Add readline
    include_directories(${READLINE_INCLUDE_DIR})
    list(APPEND LIBS ${READLINE_LIBRARY})
endif ()


# fetch lua sources from official git mirror
execute_process(COMMAND ${GIT_EXECUTABLE} clone -b ${LUA_VERSION} --depth 1 --recursive https://github.com/lua/lua.git lua WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

## SOURCES
# Sources and headers
include_directories(lua ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB SRC_LIB ${CMAKE_SOURCE_DIR}/lua/*.c)
list(REMOVE_ITEM SRC_LIB ${CMAKE_SOURCE_DIR}/lua/lua.c ${CMAKE_SOURCE_DIR}/lua/luac.c ${CMAKE_SOURCE_DIR}/lua/onelua.c)
set(SRC_LUA ${CMAKE_SOURCE_DIR}/lua/lua.c)
set(SRC_LUAC ${CMAKE_SOURCE_DIR}/lua/luac.c)

## BUILD
# Create lua library
if (BUILD_SHARED_LIBS)
    add_library(liblua SHARED ${SRC_CORE} ${SRC_LIB} ${LUA_DLL_RC})
else ()
    add_library(liblua STATIC ${SRC_CORE} ${SRC_LIB} ${LUA_DLL_RC})
endif ()
target_link_libraries(liblua ${LIBS})
set_target_properties(liblua PROPERTIES OUTPUT_NAME lua CLEAN_DIRECT_OUTPUT 1)

if (LUA_EXE)
    add_executable(lua ${SRC_LUA})
    target_link_libraries(lua liblua)

    #add_executable ( luac ${SRC_CORE} ${SRC_LIB} ${SRC_LUAC} )
    #target_link_libraries ( luac ${LIBS} )
endif ()

# On windows a variant of the lua interpreter without console output needs to be built
if (LUA_BUILD_WLUA)
    add_executable(wlua WIN32 lua/wmain.c ${SRC_LUA})
    target_link_libraries(wlua liblua)
    install_executable(wlua)
endif ()

if (LUA_EXE)
    install_executable(lua)
endif ()
install_library(liblua)
install_header(lua/lua.h lua/lualib.h lua/lauxlib.h lua/luaconf.h)

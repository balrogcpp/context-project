# This file is part of Glue Engine. Created by Andrey Vasiliev

macro(insert_dependency PACKAGE)
    find_package(${PACKAGE} QUIET)

    if (NOT ${PACKAGE}_FOUND)
        message(STATUS "${PACKAGE} required but not found")
        set(ANY_NOT_FOUND true)
    else ()
        message(STATUS "Found ${PACKAGE}")
        mark_as_advanced(${PACKAGE}_DIR)
    endif ()
endmacro()

macro(insert_dependency_static PACKAGE)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    insert_dependency(${PACKAGE})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()

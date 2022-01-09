# This source file is part of "glue project". Created by Andrey Vasiliev

macro(insert_dependency PACKAGE)
    find_package(${PACKAGE} QUIET)

    if (NOT ${PACKAGE}_FOUND)
        message(STATUS "Not found ${PACKAGE}. Please build ${PACKAGE} first")
        set(ANY_NOT_FOUND true)
    else ()
        message(STATUS "Found ${PACKAGE}")
    endif ()
endmacro()

macro(insert_dependency_static PACKAGE)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    insert_dependency(${PACKAGE})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()

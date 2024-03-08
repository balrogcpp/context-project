# include guard
if (_insertdependency_included)
    return()
endif (_insertdependency_included)
set(_insertdependency_included true)


macro(find_package_static PACKAGE VERBOSE)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    find_package(${PACKAGE} ${VERBOSE})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()


macro(insert_dependency PACKAGE)
    find_package(${PACKAGE} QUIET)

    if (NOT ${PACKAGE}_FOUND)
        message(STATUS "${PACKAGE} not found")
        set(ANY_NOT_FOUND 1)
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


macro(insert_dependency2 PACKAGE VARIABLE)
    find_package(${PACKAGE} QUIET)

    if (NOT ${VARIABLE})
        message(STATUS "${PACKAGE} not found")
        set(ANY_NOT_FOUND 1)
    else ()
        message(STATUS "Found ${PACKAGE}")
    endif ()
endmacro()


macro(insert_dependency_static2 PACKAGE VARIABLE)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    insert_dependency2(${PACKAGE} ${VARIABLE})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()

# include guard
if (_githash_included)
    return()
endif (_githash_included)
set(_githash_included true)


find_package(Git REQUIRED QUIET)


if (NOT GIT_SHA AND NOT GIT_SHA_SHORT AND GIT_FOUND AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git)
    if (EXISTS ${CMAKE_SOURCE_DIR}/.git/index)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_SHA_SHORT
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_SHA
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # try to find out HEAD hash without main repo
    elseif (EXISTS "${CMAKE_SOURCE_DIR}/.git/HEAD")
        file(READ "${CMAKE_SOURCE_DIR}/.git/HEAD" GIT_HEAD)
        string(REGEX MATCH "[^ ]+$" GIT_HEAD ${GIT_HEAD})
        string(STRIP ${GIT_HEAD} GIT_HEAD)
        if (EXISTS "${CMAKE_SOURCE_DIR}/.git/${GIT_HEAD}")
            file(READ "${CMAKE_SOURCE_DIR}/.git/${GIT_HEAD}" GIT_SHA)
            string(SUBSTRING ${GIT_SHA} 0 8 GIT_SHA_SHORT)
        endif ()
    endif ()
endif ()

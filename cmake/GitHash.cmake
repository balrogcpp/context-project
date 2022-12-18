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
    endif ()
endif ()

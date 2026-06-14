# include guard
if (_fetch_source_included)
    return()
endif (_fetch_source_included)
set(_fetch_source_included true)

set(TARGET ${TARGET})
set(REPO ${REPO})
set(TAG ${TAG})
set(PATCH ${PATCH})
set(MODULES ${MODULES})

find_package(Git REQUIRED QUIET)
include(CMakeParseArguments)
function(create_target TARGET REPO TAG)
    set(options)
    set(oneValueArgs PATCH)
    set(multiValueArgs MODULES)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Build clone configuration
    set(CLONE_ARGS --shallow-submodules -j4 --depth 1)

    if(ARG_MODULES)
        foreach(m IN LISTS ARG_MODULES)
            list(APPEND CLONE_ARGS --recurse-submodules=${m})
        endforeach()
    else()
        list(APPEND CLONE_ARGS --recursive)
    endif()

    # Patch validation
    if(PATCH)
        set(PATCH_PATH "${PATCH}")

        if(NOT EXISTS "${PATCH_PATH}")
            message(FATAL_ERROR "Patch file ${PATCH} not found")
        endif()
    endif()

    # Existing repository check
    execute_process(
            COMMAND ${GIT_EXECUTABLE} tag
            WORKING_DIRECTORY ${TARGET}
            OUTPUT_VARIABLE head_rev
            ERROR_QUIET
    )

    string(STRIP "${head_rev}" head_rev)

    file(GLOB RESULT "${TARGET}")
    list(LENGTH RESULT RES_LEN)

    set(CLONE_COMMAND
            ${GIT_EXECUTABLE} clone ${CLONE_ARGS} -b ${TAG} ${REPO} ${TARGET}
    )

    if(NOT RES_LEN EQUAL 0)
        if("${head_rev}" MATCHES "${TAG}")
            set(DOWNLOAD_PLACEHOLDER ${CMAKE_COMMAND} -E true PARENT_SCOPE)

            if(PATCH)
                execute_process(
                        COMMAND ${GIT_EXECUTABLE} reset -q --hard
                        WORKING_DIRECTORY ${TARGET}
                )

                execute_process(
                        COMMAND ${GIT_EXECUTABLE} apply -q --reject --ignore-space-change --unidiff-zero ${PATCH_PATH}
                        WORKING_DIRECTORY ${TARGET}
                )
            endif()
        else()
            file(REMOVE_RECURSE ${TARGET})
            set(DOWNLOAD_PLACEHOLDER ${CLONE_COMMAND} PARENT_SCOPE)
        endif()
    else()
        set(DOWNLOAD_PLACEHOLDER ${CLONE_COMMAND} PARENT_SCOPE)
    endif()

    # Patch handling
    if(PATCH)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} apply -q --check --ignore-space-change --unidiff-zero ${PATCH_PATH}
                WORKING_DIRECTORY ${TARGET}
                RESULT_VARIABLE checkout_err
        )

        if(NOT checkout_err
                OR NOT EXISTS ${TARGET}/.git)

            set(PATCH_PLACEHOLDER ${CMAKE_COMMAND} -E chdir ${TARGET}
                    ${GIT_EXECUTABLE} apply -q --reject --ignore-space-change --unidiff-zero ${PATCH_PATH}
            )
            set(PATCH_PLACEHOLDER ${PATCH_PLACEHOLDER} PARENT_SCOPE)
        else()
            set(PATCH_PLACEHOLDER ${CMAKE_COMMAND} -E true PARENT_SCOPE)
        endif()
    else()
        set(PATCH_PLACEHOLDER ${CMAKE_COMMAND} -E true PARENT_SCOPE)
    endif()
endfunction()

message("${CMAKE_SOURCE_DIR}")
message("${TARGET}")
message("${REPO}")
message("${TAG}")
message("${PATCH}")
message("${MODULES}")
create_target(${TARGET} ${REPO} ${TAG} ${PATCH})
execute_process(COMMAND ${DOWNLOAD_PLACEHOLDER})
execute_process(COMMAND ${PATCH_PLACEHOLDER})

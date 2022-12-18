# include guard
if (_upx_included)
    return()
endif (_upx_included)
set(_upx_included true)


find_package(SelfPackers QUIET)


# Stripping don't work on macos replaced with -g0 -s compiler flags
#if (CMAKE_STRIP AND NOT APPLE)
#    add_custom_command(
#            TARGET ${TARGET_NAME}
#            POST_BUILD
#            COMMAND ${CMAKE_STRIP} -s "$<TARGET_FILE:${TARGET_NAME}>"
#            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#            USES_TERMINAL
#    )
#endif ()


if (SELF_PACKER_FOR_EXECUTABLE AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} -9 "$<TARGET_FILE:${TARGET_NAME}>"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            USES_TERMINAL
    )
endif ()

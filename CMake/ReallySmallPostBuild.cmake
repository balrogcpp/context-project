find_package(SelfPackers)

# Stripping not work very well when crosscompile
#if (CMAKE_STRIP)
#    add_custom_command(
#            TARGET ${TARGET_NAME}
#            POST_BUILD
#            COMMAND ${CMAKE_STRIP} -s "$<TARGET_FILE:${TARGET_NAME}>"
#            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#            USES_TERMINAL
#    )
#endif ()

if (SELF_PACKER_FOR_EXECUTABLE)
    add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} -9 "$<TARGET_FILE:${TARGET_NAME}>"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            USES_TERMINAL
    )
endif ()

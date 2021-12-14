find_package(SelfPackers)

if (SELF_PACKER_FOR_EXECUTABLE)
    add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${SELF_PACKER_FOR_EXECUTABLE} --lzma "$<TARGET_FILE:${TARGET_NAME}>"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif ()

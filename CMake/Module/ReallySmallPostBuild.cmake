# This file is part of Glue Engine. Created by Andrey Vasiliev

find_package(SelfPackers)

# Stripping don't work on macosm replaced with -g0 -s compiler flags
#if (CMAKE_STRIP AND NOT APPLE)
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

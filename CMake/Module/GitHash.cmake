# This file is part of Glue Engine. Created by Andrey Vasiliev
# Define GIT_SHA1 variable

if (NOT GIT_SHA1)
    execute_process(
            COMMAND git rev-parse --short=8 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

# This file is part of Glue Engine. Created by Andrey Vasiliev

set(CMAKE_FOLDER Examples)
option(GLUE_BUILD_EXAMPLES "Build examples" ON)
if (GLUE_BUILD_EXAMPLES)
    add_subdirectory(Example0)
endif ()
set(CMAKE_FOLDER "")

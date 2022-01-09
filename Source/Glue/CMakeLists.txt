# This source file is part of "glue project". Created by Andrey Vasiliev

#option(GLUE_BUILD_STATIC_LIBRARY "Build static library" OFF)
#if (GLUE_BUILD_STATIC_LIBRARY)
#    file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES Source/*.cpp Source/*.h Source/*.hpp)
#    add_library(glue STATIC ${GLUE_ENGINE_SOURCE_FILES})
#    include_directories(glue ${GLUE_INCLUDE_DIRS})
#    set_target_properties(glue PROPERTIES OUTPUT_NAME "glue-static")
#    target_precompile_headers(glue PUBLIC Source/Glue/include/pch.h)
#endif ()
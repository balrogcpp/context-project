# This source file is part of "glue project". Created by Andrey Vasiliev

option(GLUE_BUILD_STATIC_LIBRARY "Build examples" OFF)

if (GLUE_BUILD_STATIC_LIBRARY)
    include(CppFlags)
    include(GlueBuildVariables)

    file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES src/*.cpp include/*.h include/*.hpp)
    add_library(GlueStatic STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
    include_directories(GlueStatic ${GLUE_INCLUDE_DIRS})
    set_target_properties(GlueStatic PROPERTIES OUTPUT_NAME "GlueStatic")
    include_directories(GlueStatic include ../include include/OgreOggSound include/PagedGeometry ${GLUE_INCLUDE_DIRS})
    target_precompile_headers(GlueStatic PUBLIC include/PCHeader.h)
endif ()

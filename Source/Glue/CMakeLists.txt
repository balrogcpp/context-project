# This source file is part of "glue project". Created by Andrey Vasiliev

option(GLUE_BUILD_STATIC_LIBRARY "Build examples" ON)

if (GLUE_BUILD_STATIC_LIBRARY)
    include(GlueCppFlags)
    include(GlueBuildVariables)

    file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES src/*.cpp include/*.h include/*.hpp)
    file(GLOB_RECURSE SHADER_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Programs/*)

    source_group(TREE ${GLUE_SOURCE_DIR}/Glue PREFIX "Source" FILES ${GLUE_ENGINE_SOURCE_FILES})
    source_group(TREE ${CMAKE_SOURCE_DIR}/Programs PREFIX "Assets" FILES ${SHADER_SOURCE_FILES})
    if (MSVC)
        list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
        set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE Assets)
    elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
        list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
        set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
    endif ()

    add_library(GlueStatic STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
    include_directories(GlueStatic ${GLUE_INCLUDE_DIRS})
    set_target_properties(GlueStatic PROPERTIES OUTPUT_NAME "GlueStatic")
    include_directories(GlueStatic include ${GLUE_INCLUDE_DIRS})
    target_precompile_headers(GlueStatic PUBLIC include/PCHeader.h)
endif ()

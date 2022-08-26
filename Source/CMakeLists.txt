# This file is part of Glue Engine. Created by Andrey Vasiliev


# GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS
include(GlueBuildVariables)
include(Platform)


# glob sources
file(GLOB_RECURSE SHADER_SOURCE_FILES Programs/*)


# VS project folders
source_group(TREE ${CMAKE_SOURCE_DIR}/Source/Engine PREFIX "Engine" FILES ${GLUE_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/Source/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})

if (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE Shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# Static library
add_library(GlueStatic STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
include_directories(GlueStatic ${GLUE_INCLUDE_DIRS})
set_target_properties(GlueStatic PROPERTIES OUTPUT_NAME "GlueStatic")
target_precompile_headers(GlueStatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/pch.h)

# This file is part of Glue Engine. Created by Andrey Vasiliev


# include GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS,
include(GlueBuildVariables)
include(Platform)

# glob sources
file(GLOB_RECURSE SHADER_SOURCE_FILES Programs/*)


# VS project folders
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Source PREFIX "Engine" FILES ${GLUE_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})

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
target_precompile_headers(GlueStatic PUBLIC Source/PCHeader.h)
#add_dependencies(GlueStatic Dependencies)


# Shared library
if (DESKTOP)
    add_library(GlueShared SHARED EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
    include_directories(GlueShared ${GLUE_INCLUDE_DIRS})
    set_target_properties(GlueShared PROPERTIES OUTPUT_NAME "GlueShared")
    target_precompile_headers(GlueShared PUBLIC Source/PCHeader.h)
endif ()

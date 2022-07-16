# This file is part of Glue Engine. Created by Andrey Vasiliev

add_subdirectory(ThirdParty)
set(CMAKE_PREFIX_PATH ${GLUE_THIRDPARTY_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_THIRDPARTY_ROOT})

# include GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS,
include(GlueBuildVariables)


# This will build project dependencies
if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Android/app/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy ThirdParty)
endif ()

if (ANY_NOT_FOUND)
    return()
endif ()


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


# Shared library
add_library(GlueShared SHARED EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
include_directories(GlueShared ${GLUE_INCLUDE_DIRS})
set_target_properties(GlueShared PROPERTIES OUTPUT_NAME "GlueShared")
target_precompile_headers(GlueShared PUBLIC Source/PCHeader.h)

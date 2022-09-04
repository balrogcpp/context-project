# This file is part of Glue Engine. Created by Andrey Vasiliev


# GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS
include(InsertDependency)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    insert_dependency(X11)
endif ()
if (DESKTOP)
    insert_dependency(OpenGL)
endif ()
insert_dependency_static(Bullet)
mark_as_advanced(BULLET_INCLUDE_DIR)
insert_dependency_static2(Lua LUA_LIBRARY)
insert_dependency(sol2)
insert_dependency2(OpenAL OPENAL_FOUND)
insert_dependency_static(Ogg)
insert_dependency_static(Vorbis)
insert_dependency_static(SDL2)
insert_dependency_static(Freetype)
insert_dependency_static(pugixml)
set(OGRE_STATIC 1)
insert_dependency_static(OGRE)


include(GlueBuildVariables)



if (ANY_NOT_FOUND)
    message(WARNING "Build cmake target Dependencies to get all dependencies. Then configure cmake again to enable GlueLibrary and GlueExample targets. Exiting")
endif ()


if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Engine/android/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy contrib)
    return()
endif ()


# VS project folders
file(GLOB_RECURSE SHADER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Programs/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Engine PREFIX "Engine" FILES ${GLUE_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})

if (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE Shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# Static library
add_library(engine STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
include_directories(engine ${GLUE_INCLUDE_DIRS})
set_target_properties(engine PROPERTIES OUTPUT_NAME "glue")
target_precompile_headers(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/pch.h)


# example project
add_subdirectory(Example)

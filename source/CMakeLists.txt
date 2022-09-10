# This file is part of Glue Engine. Created by Andrey Vasiliev

# GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS
include(InsertDependency)
include(CppFlags)


# deps
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


include(BuildVariables)


if (ANY_NOT_FOUND)
    message(WARNING "Build cmake target Dependencies to get all dependencies. Then configure cmake again to enable GlueLibrary and GlueExample targets. Exiting")
endif ()


if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/src/main/cpp/main.cpp)
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

# adjust flags
if (assimp_FOUND AND MSVC AND NOT RELEASE)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${GLUE_TMP_DIR}/@. --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/Engine/shell_minimal.html")
endif ()


file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.h)
# this required to to disable windows upscaling @hdp monitors @windows
if (MINGW)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/manifest.rc)
elseif (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/app.manifest)
endif ()


# Static library
add_library(engine STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
include_directories(engine ${GLUE_INCLUDE_DIRS})
set_target_properties(engine PROPERTIES OUTPUT_NAME "glue")
target_precompile_headers(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/pch.h)


# example project
file(GLOB_RECURSE SAMPLE_SOURCE_FILES Example/*.cpp Example/*.h Example/*.hpp)
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(example WIN32 ${GLUE_ENGINE_SOURCE_FILES} ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(example ${GLUE_ENGINE_SOURCE_FILES} ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(example SHARED ${GLUE_ENGINE_SOURCE_FILES} ${SAMPLE_SOURCE_FILES})
endif ()
include_directories(example include ${GLUE_INCLUDE_DIRS})
target_link_directories(example PUBLIC ${GLUE_LINK_DIRS})
target_precompile_headers(example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/pch.h)
if (ANDROID)
    set_target_properties(example PROPERTIES OUTPUT_NAME "main")
elseif(EMSCRIPTEN)
    set_target_properties(example PROPERTIES OUTPUT_NAME "index")
else()
    set_target_properties(example PROPERTIES OUTPUT_NAME "glue")
endif()

target_link_libraries(example PUBLIC ${GLUE_LIBRARIES})


#Package
if (WIN32 AND EXISTS ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    set(EXTERNAL_DLL_GLOB ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()


install(TARGETS example RUNTIME DESTINATION .)
if (EMSCRIPTEN)
    install(FILES
            "$<TARGET_FILE_DIR:example>/index.html"
            "$<TARGET_FILE_DIR:example>/index.js"
            "$<TARGET_FILE_DIR:example>/index.wasm"
            "$<TARGET_FILE_DIR:example>/index.data"
            "$<TARGET_FILE_DIR:example>/index.worker.js"
            DESTINATION .)
else ()
    install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
    install(FILES ${GLUE_TMP_DIR}/assets.zip DESTINATION .)
    install(FILES ${GLUE_TMP_DIR}/programs.zip DESTINATION .)
endif ()



set(GLUE_PACKAGE_NAME "Glue${TARGET_NAME}")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT})


if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()


if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Install/${GLUE_ARTIFACT_NAME})
endif ()

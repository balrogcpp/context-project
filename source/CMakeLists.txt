# macro insert_dependency and c++ flags, includes and libraries
include(CppFlags)
include(Platform)
include(BuildVariables)


# assets
if (NOT ANDROID)
    set(ASSETS_DIR ${CMAKE_BINARY_DIR}/assets)
else ()
    set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/.assets)
endif ()
add_custom_target(assets ALL ${CMAKE_COMMAND} -DGLSLES=${GLSLES} -DROOT_DIR=${CMAKE_SOURCE_DIR} -DASSETS_DIR=${ASSETS_DIR} -P ${CMAKE_MODULE_PATH}/Assets.cmake)


# deps
if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy contrib)
endif ()
if (ANY_NOT_FOUND)
    message(WARNING "Build cmake target contrib to get all dependencies. Then configure cmake again to enable GlueLibrary and GlueExample targets. Exiting")
    return()
endif ()


# vs project folders
file(GLOB_RECURSE ENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*)
file(GLOB_RECURSE SHADER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Programs/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Engine PREFIX "Engine" FILES ${ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})
if (MSVC)
    list(APPEND ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    list(APPEND ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# adjust flags if required
if (assimp_FOUND AND MSVC AND NOT RELEASE)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${ASSETS_DIR}/@. --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/Engine/shell_minimal.html")
endif ()


# this required to to disable windows upscaling @hdp monitors @windows
file(GLOB_RECURSE ENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.hpp)
if (MINGW)
    list(APPEND ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/manifest.rc)
elseif (MSVC)
    list(APPEND ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/app.manifest)
endif ()


# static library
add_library(engine STATIC EXCLUDE_FROM_ALL ${ENGINE_SOURCE_FILES})
include_directories(engine ${ENGINE_INCLUDE_DIRS})
target_precompile_headers(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)


# example project
file(GLOB_RECURSE SAMPLE_SOURCE_FILES Example/*.cpp Example/*.h Example/*.hpp)
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(example WIN32 ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(example ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(example SHARED ${SAMPLE_SOURCE_FILES})
endif ()
include_directories(example include ${ENGINE_INCLUDE_DIRS})
target_link_directories(example PUBLIC ${ENGINE_LINK_DIRS})
target_precompile_headers(example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
if (ANDROID)
    set_target_properties(example PROPERTIES OUTPUT_NAME "main")
elseif (EMSCRIPTEN)
    set_target_properties(example PROPERTIES OUTPUT_NAME "index")
else ()
    set_target_properties(example PROPERTIES OUTPUT_NAME "app")
endif ()
target_link_libraries(example PUBLIC ${ENGINE_LIBRARIES} engine)
if (ANDROID)
    add_dependencies(example assets)
endif ()


# package
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${DEPS_ROOT}/bin/*.dll)
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
    install(FILES ${ASSETS_DIR}/assets.zip DESTINATION .)
    install(FILES ${ASSETS_DIR}/programs.zip DESTINATION .)
endif ()
set(PACKAGE_NAME "Glue${TARGET_NAME}")
set(ARTIFACT_NAME ${PACKAGE_NAME}_${TOOLCHAIN_SHORT})
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    string(APPEND ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()

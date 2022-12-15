# macro insert_dependency and c++ flags, includes and libraries
include(CppFlags)
include(Platform)
set(CMAKE_FIND_ROOT_PATH ${DEPS_ROOT})
set(CMAKE_PREFIX_PATH ${DEPS_ROOT})
include(BuildVariables)


# assets
if (NOT ANDROID)
set(ASSETS_DIR ${CMAKE_BINARY_DIR}/assets)
else ()
    set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/assets)
endif ()

add_custom_target(Programs ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Programs -DOUTPUT=${ASSETS_DIR}/programs -DFORMAT=zip -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
                         COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} ${CMAKE_CXX_COMPILER} ${CMAKE_SOURCE_DIR}/source/Tools/Zip2Cpp.cpp -o zip2cpp
                         COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} zip2cpp programs
                         COMMAND ${CMAKE_COMMAND} -E copy ${ASSETS_DIR}/programs.zip.h ${CMAKE_SOURCE_DIR}/source/Engine/Code)

add_custom_target(Assets ALL ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Example/Assets -DOUTPUT=${ASSETS_DIR}/assets -DFORMAT=bin -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
                         COMMAND ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Programs -DOUTPUT=${ASSETS_DIR}/programs -DFORMAT=bin -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake)

# deps
if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy contrib)
    message(WARNING "Not all dependencies are available. Build will fails, please build contrib target first (cmake --build . --target contrib). Exiting...")
    return()
endif ()
if (ANY_NOT_FOUND)
    message(WARNING "Not all dependencies are available. Build will fails, please build contrib target first (cmake --build . --target contrib)")
endif ()


# this required to to disable windows upscaling @hdp monitors @windows
file(GLOB_RECURSE ENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.hpp)
if (MINGW)
    list(APPEND ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/manifest.rc)
elseif (MSVC)
    list(APPEND ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/app.manifest)
endif ()


# engine vs project folders
file(GLOB_RECURSE SHADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Programs/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Engine PREFIX "Engine" FILES ${ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Programs PREFIX "Programs" FILES ${SHADER_FILES})
list(APPEND ENGINE_SOURCE_FILES ${SHADER_FILES})
if (MSVC)
    set_source_files_properties(${SHADER_FILES} PROPERTIES LANGUAGE shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set_source_files_properties(${SHADER_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# adjust flags if required
if (MSVC AND RELWITHDEBINFO)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /SUBSYSTEM:WINDOWS")
endif ()
if (MSVC AND assimp_FOUND AND DEBUG)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()
if (EMSCRIPTEN)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${ASSETS_DIR}/@. --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/Engine/shell_minimal.html")
endif ()


# static library
add_library(Engine STATIC EXCLUDE_FROM_ALL ${ENGINE_SOURCE_FILES})
include_directories(Engine ${ENGINE_INCLUDE_DIRS})
target_precompile_headers(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
add_dependencies(Engine Programs)


# Example vs project folders
file(GLOB_RECURSE SAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Example/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Example/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Example/*.hpp)
file(GLOB_RECURSE ASSET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Example/Assets/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Example/Source PREFIX "Example" FILES ${SAMPLE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Example/Assets PREFIX "Assets" FILES ${ASSET_FILES})
list(APPEND SAMPLE_SOURCE_FILES ${ASSET_FILES})
if (MSVC)
    set_source_files_properties(${ASSET_FILES} PROPERTIES LANGUAGE shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set_source_files_properties(${ASSET_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# executable
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
        add_executable(Example WIN32 ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(Example ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(Example SHARED ${SAMPLE_SOURCE_FILES})
endif ()
include_directories(Example include ${ENGINE_INCLUDE_DIRS})
target_link_directories(Example PUBLIC ${ENGINE_LINK_DIRS})
target_precompile_headers(Example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
if (ANDROID)
    set_target_properties(Example PROPERTIES OUTPUT_NAME "main")
elseif (EMSCRIPTEN)
    set_target_properties(Example PROPERTIES OUTPUT_NAME "index")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else ()
    set_target_properties(Example PROPERTIES OUTPUT_NAME "app")
endif ()
target_link_libraries(Example PUBLIC Engine ${ENGINE_LIBRARIES})
if (ANDROID)
    add_dependencies(Example Assets)
endif ()


# package
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${DEPS_ROOT}/bin/*.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()
install(TARGETS Example RUNTIME DESTINATION .)
if (EMSCRIPTEN)
    install(FILES
            "$<TARGET_FILE_DIR:Example>/index.html"
            "$<TARGET_FILE_DIR:Example>/index.js"
            "$<TARGET_FILE_DIR:Example>/index.wasm"
            "$<TARGET_FILE_DIR:Example>/index.data"
            "$<TARGET_FILE_DIR:Example>/index.worker.js"
            DESTINATION .)
else ()
    install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
    install(FILES ${ASSETS_DIR}/Assets.bin DESTINATION .)
    #install(FILES ${ASSETS_DIR}/programs.bin DESTINATION .)
endif ()
set(PACKAGE_NAME "Glue${TARGET_NAME}")
set(ARTIFACT_NAME ${PACKAGE_NAME}_${TOOLCHAIN_SHORT})
if (NOT CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    string(APPEND ARTIFACT_NAME "_${CMAKE_BUILD_TYPE_LOWER}")
endif ()

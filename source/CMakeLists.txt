# project
project(COMPILE)


# macro insert_dependency and c++ flags, includes and libraries
include(CppFlags)
include(Platform)
include(TimeStampEraser)
include(BuildVariables)
include(DateTime)


# assets
set(ASSETS_DIR ${CMAKE_BINARY_DIR}/assets)
set(ASSETS_DIR_ANDROID ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/assets)
make_directory(${ASSETS_DIR_ANDROID})

if (NOT CPP_COMPILER)
    if (CMAKE_CROSSCOMPILING)
        find_program(CPP_COMPILER NAMES c++ clang++ g++ cl bcc32 bcc xlc PATHS ENV PATH NO_DEFAULT_PATH)
    else ()
        set(CPP_COMPILER ${CMAKE_CXX_COMPILER})
    endif ()
endif ()
if (NOT CPP_COMPILER)
    message(FATAL_ERROR "CXX compiler not found manually")
else ()
    message(STATUS "CXX compiler found manually: ${CPP_COMPILER}")
endif ()

if (RELEASE)
    add_custom_target(Programs ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Programs -DOUTPUT=${ASSETS_DIR}/programs -DFORMAT=zip -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
            COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} ${CPP_COMPILER} ${CMAKE_SOURCE_DIR}/source/Tools/Zip2Cpp.cpp -o zip2cpp
            COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} ./zip2cpp programs
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/programs.zip.h ${CMAKE_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${ASSETS_DIR}/programs.zip)

    add_custom_target(Assets ALL ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Demo/Assets -DOUTPUT=${ASSETS_DIR}/assets -DFORMAT=bin -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${ASSETS_DIR_ANDROID}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSETS_DIR_ANDROID}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/assets.bin ${ASSETS_DIR_ANDROID})
else ()
    add_custom_target(Assets ALL ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Demo/Assets -DOUTPUT=${ASSETS_DIR}/assets -DFORMAT=bin -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
            COMMAND ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Programs -DOUTPUT=${ASSETS_DIR}/programs -DFORMAT=bin -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/assets.bin ${ASSETS_DIR_ANDROID}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/programs.bin ${ASSETS_DIR_ANDROID})
endif ()


# deps
if (ANDROID AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Engine/android/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy Contrib)
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
endif ()
if (EMSCRIPTEN)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${ASSETS_DIR}/@. --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/Engine/shell_minimal.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()


# static library
add_library(Engine STATIC EXCLUDE_FROM_ALL ${ENGINE_SOURCE_FILES})
include_directories(Engine ${ENGINE_INCLUDE_DIRS})
target_precompile_headers(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
if (RELEASE)
    add_dependencies(Engine Programs)
endif ()


# Demo vs project folders
file(GLOB_RECURSE SAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Demo/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Demo/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Demo/*.hpp)
file(GLOB_RECURSE ASSET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Demo/Assets/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Demo/Source PREFIX "Demo" FILES ${SAMPLE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Demo/Assets PREFIX "Assets" FILES ${ASSET_FILES})
list(APPEND SAMPLE_SOURCE_FILES ${ASSET_FILES})
if (MSVC)
    set_source_files_properties(${ASSET_FILES} PROPERTIES LANGUAGE shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set_source_files_properties(${ASSET_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# executable
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
        add_executable(Demo WIN32 ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(Demo ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(Demo SHARED ${SAMPLE_SOURCE_FILES})
endif ()
include_directories(Demo include ${ENGINE_INCLUDE_DIRS})
target_link_directories(Demo PUBLIC ${ENGINE_LINK_DIRS})
target_precompile_headers(Demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
set_target_properties(Demo PROPERTIES OUTPUT_NAME "app")
target_link_libraries(Demo PUBLIC Engine ${ENGINE_LIBRARIES})
if (ANDROID)
    add_dependencies(Demo Assets)
endif ()


# post-build, pre-package
#if (RELEASE)
#    set(TARGET_NAME "Demo")
#    include(ReallySmallPostBuild)
#endif ()


# install
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${DEPS_ROOT}/bin/*.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()
install(TARGETS Demo RUNTIME DESTINATION .)
if (NOT EMSCRIPTEN)
    install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
    install(FILES ${ASSETS_DIR}/assets.bin DESTINATION .)
    if (NOT RELEASE)
        install(FILES ${ASSETS_DIR}/programs.bin DESTINATION .)
    endif ()
else ()
    set(FILE_DIR $<TARGET_FILE_DIR:Demo>)
    install(FILES "${FILE_DIR}/app.html" "${FILE_DIR}/app.js" "${FILE_DIR}/app.wasm" "${FILE_DIR}/app.data" "${FILE_DIR}/app.worker.js" DESTINATION .)
endif ()


# package
if (RELEASE)
    set(CPACK_INSTALL_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/TimeStampEraser.cmake")
    set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/CpackPreBuild.cmake")
    set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/CpackPostBuild.cmake")
endif ()

# android
if (UNIX)
    set(SH "sh")
endif ()

add_custom_target(GradleContrib COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Engine ${CMAKE_COMMAND} -E env GIT_SHA1=${GIT_SHA_SHORT} ${SH} gradlew assemble${CMAKE_BUILD_TYPE_NORMALISED}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Engine ${CMAKE_COMMAND} -E rm -rf build .gradle android/.cxx android/build android/assets)

add_custom_target(GradleBuild COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Engine ${CMAKE_COMMAND} -E env GIT_SHA1=${GIT_SHA_SHORT} GRADLE_DEPLOY=1 ${SH} gradlew assemble${CMAKE_BUILD_TYPE_NORMALISED})

#cpack
set(PACKAGE_NAME "demo")
set(ARTIFACT_NAME ${PACKAGE_NAME}-${GIT_SHA_SHORT}-${TOOLCHAIN_SHORT_NORMALISED}-${DATE_TIME_CPACK})
if (NOT RELEASE)
    string(APPEND ARTIFACT_NAME "-${CMAKE_BUILD_TYPE_LOWER}")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
elseif (WIN32)
    install(FILES DESTINATION .)
endif ()
set(CPACK_PACKAGE_DIRECTORY ${ARTIFACT_DIR})
set(CPACK_PACKAGE_FILE_NAME ${ARTIFACT_NAME})
if (UNIX)
    set(CPACK_GENERATOR "TGZ")
else ()
    set(CPACK_GENERATOR "ZIP")
endif ()
include(CPack)

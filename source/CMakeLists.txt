# macro insert_dependency and c++ flags, includes and libraries
include(CppFlags)
include(Platform)
include(BuildVariables)
include(DateTime)


# this required to to disable windows upscaling @hdp monitors @windows
file(GLOB_RECURSE ENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.hpp)

# engine vs project folders
file(GLOB_RECURSE SHADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Programs/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Engine PREFIX "Engine" FILES ${ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Programs PREFIX "Programs" FILES ${SHADER_FILES})
list(APPEND ENGINE_SOURCE_FILES ${SHADER_FILES})
if (MSVC)
    set_source_files_properties(${SHADER_FILES} PROPERTIES LANGUAGE resource)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode")
    set_source_files_properties(${SHADER_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()

# adjust flags if required
if (MSVC AND RELWITHDEBINFO)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /SUBSYSTEM:WINDOWS")
endif ()
# link fix for old version of assimp
#find_package(assimp QUIET)
#if (MSVC AND assimp_FOUND AND assimp_VERSION VERSION_LESS 5.2)
#    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
#endif ()
if (EMSCRIPTEN)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " --preload-file ${ASSETS_DIR}/@. --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/Engine/shell_minimal.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

# static library
add_library(Engine STATIC EXCLUDE_FROM_ALL ${ENGINE_SOURCE_FILES})
include_directories(Engine ${ENGINE_INCLUDE_DIRS})
target_precompile_headers(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
add_custom_target(Programs COMMAND ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Programs -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/programs.zip -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
                            COMMAND ${CMAKE_COMMAND} -DINPUT=${CMAKE_CURRENT_BINARY_DIR}/programs.zip -P ${CMAKE_SOURCE_DIR}/cmake/Zip2Cpp.cmake)
set_target_properties(Programs PROPERTIES FOLDER "Helpers")
add_dependencies(Engine Programs)

# tests vs project folders
file(GLOB_RECURSE SAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.hpp)
file(GLOB_RECURSE ASSET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Assets/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Source PREFIX "Tests" FILES ${SAMPLE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Assets PREFIX "Assets" FILES ${ASSET_FILES})
list(APPEND SAMPLE_SOURCE_FILES ${ASSET_FILES})
if (MSVC)
    set_source_files_properties(${ASSET_FILES} PROPERTIES LANGUAGE shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set_source_files_properties(${ASSET_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()
if (MINGW)
    list(APPEND SAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/manifest.rc)
elseif (MSVC)
    list(APPEND SAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/app.manifest)
endif ()

# executable
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
        add_executable(Tests WIN32 ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(Tests ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(Tests SHARED ${SAMPLE_SOURCE_FILES})
endif ()
include_directories(Tests include ${ENGINE_INCLUDE_DIRS})
target_link_directories(Tests PUBLIC ${ENGINE_LINK_DIRS})
target_precompile_headers(Tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Code/pch.h)
set_target_properties(Tests PROPERTIES OUTPUT_NAME "app")
target_link_libraries(Tests PUBLIC Engine ${ENGINE_LIBRARIES})

if (ANDROID)
    add_custom_target(Assets ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Tests/Assets -DOUTPUT=${CMAKE_CURRENT_SOURCE_DIR}/Android/app/assets/assets.zip -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake)
    add_dependencies(Tests Assets)
else ()
    add_custom_target(Assets ALL ${CMAKE_COMMAND} -DINPUT=${CMAKE_SOURCE_DIR}/source/Tests/Assets -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/assets.zip -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake)
    set_target_properties(Assets PROPERTIES FOLDER "Helpers")
endif ()

# install
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${DEPS_ROOT}/bin/*.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()
install(TARGETS Tests RUNTIME DESTINATION .)
if (NOT EMSCRIPTEN)
    install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/assets.zip DESTINATION .)
else ()
    set(FILE_DIR $<TARGET_FILE_DIR:Tests>)
    install(FILES "${FILE_DIR}/app.html" "${FILE_DIR}/app.js" "${FILE_DIR}/app.wasm" "${FILE_DIR}/app.data" "${FILE_DIR}/app.worker.js" DESTINATION .)
endif ()

# cpack
set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/CpackPreBuild.cmake")
set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/CpackPostBuild.cmake")
set(PACKAGE_NAME "tests")
# set(ARTIFACT_NAME ${PACKAGE_NAME}-${GIT_SHA_SHORT}-${TOOLCHAIN_SHORT_NORMALISED}-${DATE_TIME_CPACK})
set(ARTIFACT_NAME ${PACKAGE_NAME}-${GIT_SHA_SHORT}-${TOOLCHAIN_FULL}-${CMAKE_BUILD_TYPE_LOWER})
if (LINUX)
    install(FILES PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
else ()
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

# android
if (WIN32)
    set(GRADLEW_COMMAND gradlew.bat)
else ()
    set(GRADLEW_COMMAND sh gradlew)
endif ()
if (CMAKE_BUILD_TYPE_LOWER MATCHES debug)
    set(GRADLE_BUILD_TYPE "debug")
else ()
    set(GRADLE_BUILD_TYPE "release")
endif ()

add_custom_target(Gradle
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Android ${GRADLEW_COMMAND} assemble${GRADLE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Android ${GRADLEW_COMMAND} clean
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Android ${GRADLEW_COMMAND} assemble${GRADLE_BUILD_TYPE}
)
add_custom_target(GradleClear COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/Android ${CMAKE_COMMAND} -E rm -rf .gradle app/.cxx app/build app/assets)
set_target_properties(Gradle GradleClear PROPERTIES FOLDER "Helpers")

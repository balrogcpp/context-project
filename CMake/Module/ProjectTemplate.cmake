# This file is part of Glue Engine. Created by Andrey Vasiliev


include(GlueBuildVariables)


# glob sources
file(GLOB_RECURSE SAMPLE_SOURCE_FILES Source/*.cpp Source/*.h Source/*.hpp)
file(GLOB_RECURSE ASSETS_SOURCE_FILES Assets/*)
file(GLOB_RECURSE SHADER_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Engine/Programs/*)


# VS project folders
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Source PREFIX "Engine" FILES ${GLUE_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})
source_group(TREE ${GLUE_PROJECT_SOURCE_DIR}/Source PREFIX "Source" FILES ${SAMPLE_SOURCE_FILES})
source_group(TREE ${GLUE_PROJECT_SOURCE_DIR}/Assets PREFIX "Assets" FILES ${ASSETS_SOURCE_FILES})


if (MSVC)
    list(APPEND SAMPLE_SOURCE_FILES ${ASSETS_SOURCE_FILES})
    set_source_files_properties(${ASSETS_SOURCE_FILES} PROPERTIES LANGUAGE Assets)
    list(APPEND SAMPLE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE Shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    list(APPEND SAMPLE_SOURCE_FILES ${ASSETS_SOURCE_FILES})
    set_source_files_properties(${ASSETS_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
    list(APPEND SAMPLE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


list(APPEND SAMPLE_SOURCE_FILES ${GLUE_ENGINE_SOURCE_FILES})
if (NOT ANDROID)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(${TARGET_NAME} WIN32 ${SAMPLE_SOURCE_FILES})
    else ()
        add_executable(${TARGET_NAME} ${SAMPLE_SOURCE_FILES})
    endif ()
else ()
    add_library(${TARGET_NAME} SHARED ${SAMPLE_SOURCE_FILES})
endif ()


include_directories(${TARGET_NAME} include ${GLUE_INCLUDE_DIRS})
target_link_directories(${TARGET_NAME} PUBLIC ${GLUE_LINK_DIRS})
target_precompile_headers(${TARGET_NAME} PUBLIC ${GLUE_SOURCE_DIR}/PCHeader.h)
if (NOT ANDROID)
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}")
else ()
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "main")
endif ()

target_link_libraries(${TARGET_NAME} PUBLIC ${GLUE_LINK_LIBRARIES})

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION .)


# use upx to reduce binary size
if (${CMAKE_BUILD_TYPE} STREQUAL "Release" AND DESKTOP)
    include(ReallySmallPostBuild)
endif ()


#Package
if (WIN32 AND EXISTS ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    set(EXTERNAL_DLL_GLOB ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()


set(GLUE_PACKAGE_NAME "Glue${TARGET_NAME}")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT}_${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()
if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Install/${GLUE_ARTIFACT_NAME})
endif ()
install(FILES ${EXTERNAL_DLL_GLOB} DESTINATION .)
install(FILES ${GLUE_TMP_DIR}/Assets.zip DESTINATION .)
install(DIRECTORY ${GLUE_TMP_DIR}/Programs DESTINATION .)

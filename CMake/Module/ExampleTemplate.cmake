# This source file is part of "glue project". Created by Andrey Vasiliev

include(CppFlags)
include(GlueBuildVariables)

set(GLUE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
file(GLOB_RECURSE SAMPLE_SOURCE_FILES ${GLUE_SOURCE_DIR}/Glue/src/*.cpp src/*.cpp ${GLUE_SOURCE_DIR}/Glue/include/*.h include/*.h ${GLUE_SOURCE_DIR}/Glue/include/*.hpp include/*.hpp)
if (MINGW)
    list(APPEND SAMPLE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Source/manifest.rc)
elseif (MSVC)
    list(APPEND SAMPLE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Source/app.manifest)
endif ()

if (NOT ANDROID)
    add_executable(${TARGET_NAME} WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(${TARGET_NAME} SHARED ${SAMPLE_SOURCE_FILES})
endif ()

include_directories(${TARGET_NAME} include ${GLUE_SOURCE_DIR}/Glue/include ${GLUE_SOURCE_DIR}/Glue/include/OgreOggSound ${GLUE_SOURCE_DIR}/Glue/include/PagedGeometry ${GLUE_SOURCE_DIR}/include ${GLUE_INCLUDE_DIRS})
target_link_directories(${TARGET_NAME} PUBLIC ${GLUE_LINK_DIRS})
target_precompile_headers(${TARGET_NAME} PUBLIC ${GLUE_SOURCE_DIR}/Glue/include/PCHeader.h)
if (NOT ANDROID)
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}")
else ()
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "main")
endif ()

target_link_libraries(${TARGET_NAME} PUBLIC ${GLUE_LINK_LIBRARIES})

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION .)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release" AND DESKTOP AND NOT APPLE)
    include(ReallySmallPostBuild)
endif ()

#Packaging
set(GLUE_DEPLOY_DIR ${CMAKE_SOURCE_DIR}/Deploy)

if (WIN32)
    set(EXTERNAL_DLL_GLOB ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()

set(GLUE_PACKAGE_NAME "GlueSample")
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

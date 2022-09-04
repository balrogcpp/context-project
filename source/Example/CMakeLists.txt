# This file is part of Glue Engine. Created by Andrey Vasiliev


include(GlueBuildVariables)



file(GLOB_RECURSE SAMPLE_SOURCE_FILES *.cpp *.h *.hpp)
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
target_precompile_headers(example PUBLIC ${GLUE_SOURCE_DIR}/pch.h)
if (NOT ANDROID)
    set_target_properties(example PROPERTIES OUTPUT_NAME "example")
else ()
    set_target_properties(example PROPERTIES OUTPUT_NAME "main")
endif ()


target_link_libraries(example PUBLIC ${GLUE_LIBRARIES})


#Package
if (WIN32 AND EXISTS ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    set(EXTERNAL_DLL_GLOB ${GLUE_THIRDPARTY_ROOT}/bin/OpenAL32.dll)
    file(COPY ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()


install(TARGETS example RUNTIME DESTINATION .)
if (EMSCRIPTEN)
    install(FILES
            "$<TARGET_FILE_DIR:example>/example.html"
            "$<TARGET_FILE_DIR:example>/example.js"
            "$<TARGET_FILE_DIR:example>/example.wasm"
            "$<TARGET_FILE_DIR:example>/example.data"
            "$<TARGET_FILE_DIR:example>/example.worker.js"
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

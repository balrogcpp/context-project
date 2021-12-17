include(CppFlags)

include(GlueBuildVariables)

# In assimp version is >5.1.3 pugixml is insourced and cause linking error
#if (MSVC AND NOT ASSIMP_IRRXML_LIBRARY)
#    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
#endif ()

set(TARGET_NAME Sample)

file(GLOB_RECURSE SAMPLE_SOURCE_FILES ../Glue/src/*.cpp src/*.cpp ../Glue/include/*.h include/*.h ../Glue/include/*.hpp include/*.hpp)
if (MSVC)
    list(APPEND SAMPLE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Deploy/${TARGET_NAME}.exe.manifest)
endif ()

if (NOT ANDROID)
    add_executable(${TARGET_NAME} WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(${TARGET_NAME} SHARED ${SAMPLE_SOURCE_FILES})
endif ()

include_directories(${TARGET_NAME} include ../Glue/include ../Glue/include/OgreOggSound ../Glue/include/PagedGeometry ../include ${GLUE_INCLUDE_DIRS})
target_link_directories(${TARGET_NAME} PUBLIC ${GLUE_LINK_DIRS})
target_precompile_headers(${TARGET_NAME} PUBLIC ../Glue/include/pch.h)
if (NOT ANDROID)
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}")
else ()
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "main")
endif ()

target_link_libraries(${TARGET_NAME} PUBLIC ${GLUE_LINK_LIBRARIES})

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION .)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release" AND NOT ANDROID)
    include(ReallySmallPostBuild)
endif ()

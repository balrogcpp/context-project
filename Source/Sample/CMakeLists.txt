include(CppFlags)

include(GlueBuildVariables)

# In assimp version is >5.1.3 pugixml is insourced and cause linking error
#if (MSVC AND NOT ASSIMP_IRRXML_LIBRARY)
#    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
#endif ()

if (NOT ANDROID)
    add_executable(Sample WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(Sample SHARED ${SAMPLE_SOURCE_FILES})
endif ()

include_directories(Sample include ../Glue/include ../Glue/include/OgreOggSound ../Glue/include/PagedGeometry ${GLUE_INCLUDE_DIRS})
target_link_directories(Sample PUBLIC ${GLUE_LINK_DIRS})
target_precompile_headers(Sample PUBLIC ../Glue/include/pch.h)
if (NOT ANDROID)
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "Sample")
else ()
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "main")
endif ()


target_link_libraries(Sample PUBLIC ${GLUE_LINK_LIBRARIES})

install(TARGETS Sample RUNTIME DESTINATION .)

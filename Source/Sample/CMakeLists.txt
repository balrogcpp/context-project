include(CppFlags)

set(GLUE_LIB_DIR ${CMAKEBINARY_DIR}/glue)

list(APPEND GLUE_INCLUDE_DIRS
        ${GLUE_EXTERNAL_INSTALL_LOCATION}/include
        ${OPENAL_INCLUDE_DIR}
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_INCLUDE_DIRS}/Overlay
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES
        ${OGRE_INCLUDE_DIRS}/Plugins/Assimp
        )

file(GLOB_RECURSE SAMPLE_SOURCE_FILES ../Glue/src/*.cpp src/*.cpp ../Glue/include/*.h include/*.h ../Glue/include/*.hpp include/*.hpp)

if (NOT ANDROID)
    add_executable(Sample WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(Sample SHARED ${SAMPLE_SOURCE_FILES})
endif ()

include_directories(Sample include ../Glue/include ../Glue/include/OgreOggSound ../Glue/include/PagedGeometry ${GLUE_INCLUDE_DIRS})
target_link_directories(Sample PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
target_precompile_headers(Sample PUBLIC ../Glue/include/pch.h)
if (NOT ANDROID)
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "Sample")
else ()
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "main")
endif ()
if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #jemalloc fix problems with memory bloat on linux
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl jemalloc.a)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES SDL2::hidapi android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()
if (NOT (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic)
else ()
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic_d)
endif ()

target_link_libraries(Sample PUBLIC
        ${OGRE_LIBRARIES}
        ${OGRE_CODEC_ASSIMP}
        ${ASSIMP_LIBRARIES}
        ${ASSIMP_IRRXML_LIBRARY}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        vorbisfile
        vorbis
        vorbisenc
        Ogg::ogg
        pugixml
        SDL2::SDL2-static
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

install(TARGETS Sample RUNTIME DESTINATION .)

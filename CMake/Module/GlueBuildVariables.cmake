include(Platform)

set(GLUE_INCLUDE_DIRS
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
        ${OGRE_INCLUDE_DIRS}/Plugins/DotScene
        )

set(GLUE_LINK_DIRS ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE)

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (LINUX)
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES SDL2::hidapi android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()

#jemalloc can fix problems with memory bloat on linux
#if (LINUX)
#    list(APPEND SYSTEM_LIBRARIES jemalloc.a)
#endif ()
if (NOT (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic)
else ()
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic_d)
endif ()

if (ASSIMP_IRRXML_LIBRARY)
    list(APPEND ASSIMP_LIBRARIES ${ASSIMP_IRRXML_LIBRARY})
endif ()

set(GLUE_LINK_LIBRARIES
        ${OGRE_LIBRARIES}
        ${OGRE_DOT_SCENE}
        ${OGRE_CODEC_ASSIMP}
        pugixml
        ${ASSIMP_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        vorbisfile
        vorbis
        vorbisenc
        Ogg::ogg
        SDL2::SDL2-static
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

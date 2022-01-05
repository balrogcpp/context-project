include(Platform)

set(OGRE_INCLUDE_DIR ${OGRE_INCLUDE_DIRS})

list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Overlay)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GL3Plus)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/GLSLES)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/Assimp)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/DotScene)

set(GLUE_INCLUDE_DIRS
        ${GLUE_EXTERNAL_INSTALL_LOCATION}/include
        ${OPENAL_INCLUDE_DIR}
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
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

set(VORBIS_LIBRARIES "vorbisfile;vorbis;vorbisenc")

set(GLUE_LINK_LIBRARIES
        ${OGRE_LIBRARIES}
        pugixml
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        ${VORBIS_LIBRARIES}
        Ogg::ogg
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        SDL2::SDL2-static
        ${SYSTEM_LIBRARIES}
        )

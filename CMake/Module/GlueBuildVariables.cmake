include(Platform)

set(GLUE_INCLUDE_DIRS
        ${GLUE_EXTERNAL_INSTALL_LOCATION}/include
        ${OPENAL_INCLUDE_DIR}
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        )

set(GLUE_LINK_DIRS ${GLUE_EXTERNAL_INSTALL_LOCATION}/lib ${GLUE_EXTERNAL_INSTALL_LOCATION}/lib/OGRE)

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (LINUX)
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()

#jemalloc can fix problems with memory bloat on linux
#if (LINUX)
#    list(APPEND SYSTEM_LIBRARIES jemalloc.a)
#endif ()

set(VORBIS_LIBRARIES "vorbisfile;vorbis;vorbisenc")

if (SDL2_FOUND)
    set(SDL2_LIBRARIES SDL2::SDL2-static)
    if (ANDROID)
        list(APPEND SDL2_LIBRARIES SDL2::hidapi)
    endif ()
endif ()

set(GLUE_LINK_LIBRARIES
        ${OGRE_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        ${VORBIS_LIBRARIES}
        ${OGG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SDL2_LIBRARIES}
        ${SYSTEM_LIBRARIES}
        )

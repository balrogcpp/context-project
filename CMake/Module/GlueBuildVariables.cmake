# This file is part of Glue Engine. Created by Andrey Vasiliev

include(Platform)

if (assimp_FOUND AND MSVC AND NOT RELEASE)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()

set(GLUE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)

set(GLUE_INCLUDE_DIRS
        ${GLUE_SOURCE_DIR}/include
        ${GLUE_SOURCE_DIR}/Glue/include
        ${GLUE_SOURCE_DIR}/Glue/include/OggSound
        ${GLUE_SOURCE_DIR}/Glue/include/PagedGeometry
        ${GLUE_SOURCE_DIR}/Glue/include/Caelum
        ${GLUE_SOURCE_DIR}/Glue/include/Procedural
        ${GLUE_THIRDPARTY_ROOT}/include
        ${OPENAL_INCLUDE_DIR}
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        )

set(GLUE_LINK_DIRS ${GLUE_THIRDPARTY_ROOT}/lib ${GLUE_THIRDPARTY_ROOT}/lib/OGRE)

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (LINUX)
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()

if (SDL2_FOUND)
    set(SDL2_LIBRARIES SDL2::SDL2-static)
endif ()

set(GLUE_LINK_LIBRARIES
        ${OGRE_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        ${VORBIS_LIBRARIES}
        ${OGG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

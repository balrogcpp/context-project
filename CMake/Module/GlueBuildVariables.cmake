# This file is part of Glue Engine. Created by Andrey Vasiliev

# GLUE_SOURCE_DIR, GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS, GLUE_LINK_DIRS, GLUE_LINK_LIBRARIES


include(Platform)
include(InsertDependency)
include(AppleThreadFix)
include(GlueCppFlags)


if (assimp_FOUND AND MSVC AND NOT RELEASE)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()


set(GLUE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Engine/Source)


set(GLUE_INCLUDE_DIRS
        ${GLUE_SOURCE_DIR}
        ${GLUE_SOURCE_DIR}/Engine
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
elseif (APPLE)
    list(APPEND SYSTEM_LIBRARIES z)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()

find_package(SDL2 QUIET)
if (SDL2_FOUND)
    set(SDL2_LIBRARIES SDL2::SDL2-static)
endif ()
find_package(pugixml QUIET)
if (pugixml_FOUND)
    set(PUGIXML_LIBRARIES pugixml::static)
endif ()


set(GLUE_LINK_LIBRARIES
        ${OGRE_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${PUGIXML_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        ${VORBIS_LIBRARIES}
        ${OGG_LIBRARIES}
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES ${GLUE_SOURCE_DIR}/*.cpp ${GLUE_SOURCE_DIR}/*.h ${GLUE_SOURCE_DIR}/*.hpp)

if (MINGW)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Engine/Source/manifest.rc)
elseif (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Engine/Source/app.manifest)
endif ()

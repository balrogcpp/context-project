# This file is part of Glue Engine. Created by Andrey Vasiliev

# GLUE_SOURCE_DIR, GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS, GLUE_LINK_DIRS, GLUE_LIBRARIES


include(Platform)
include(GlueCppFlags)


if (assimp_FOUND AND MSVC AND NOT RELEASE)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /FORCE:MULTIPLE")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " /FORCE:MULTIPLE")
endif ()


# build html as emscripten
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -s FULL_ES3 --preload-file ${GLUE_TMP_DIR}/@. --shell-file ${CMAKE_SOURCE_DIR}/source/Engine/shell_minimal.html")
    string(APPEND CMAKE_CXX_FLAGS " -s FULL_ES3")
    string(APPEND CMAKE_C_FLAGS " -s FULL_ES3")
endif ()


set(GLUE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/source/Engine)


set(GLUE_INCLUDE_DIRS
        ${GLUE_SOURCE_DIR}
        ${GLUE_SOURCE_DIR}/Code
        ${GLUE_THIRDPARTY_ROOT}/include
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


if (OGRE_FOUND)
    list(APPEND GLUE_LIBRARIES ${OGRE_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS})
endif ()
if (TARGET SDL2::SDL2-static)
    list(APPEND GLUE_LIBRARIES SDL2::SDL2-static)
    list(APPEND GLUE_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
endif ()
if (TARGET pugixml::static)
    list(APPEND GLUE_LIBRARIES pugixml::static)
    list(APPEND GLUE_INCLUDE_DIRS ${PUGIXML_INCLUDE_DIRS})
endif ()
if (Bullet_FOUND)
    list(APPEND GLUE_LIBRARIES ${BULLET_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
endif ()
if (Vorbis_FOUND)
    list(APPEND GLUE_LIBRARIES ${VORBIS_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS})
endif ()
if (Ogg_FOUND)
    list(APPEND GLUE_LIBRARIES ${OGG_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${OGG_INCLUDE_DIRS})
endif ()
if (OpenAL_FOUND)
    list(APPEND GLUE_LIBRARIES ${OPENAL_LIBRARY})
    list(APPEND GLUE_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
endif ()
if (Lua_FOUND)
    list(APPEND GLUE_LIBRARIES ${LUA_LIBRARY})
    list(APPEND GLUE_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
endif ()


list(APPEND GLUE_LIBRARIES ${SYSTEM_LIBRARIES})


file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES ${GLUE_SOURCE_DIR}/*.cpp ${GLUE_SOURCE_DIR}/*.h ${GLUE_SOURCE_DIR}/*.hpp)
# this required to ti disable windows upscaling @hdp monitors @windows
if (MINGW)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/manifest.rc)
elseif (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/Engine/app.manifest)
endif ()

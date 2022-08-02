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


# build html as emscripten
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -s FULL_ES3 --preload-file ${GLUE_TMP_DIR}/@. --shell-file ${CMAKE_SOURCE_DIR}/Engine/Source/shell_minimal.html ")
    string(APPEND CMAKE_CXX_FLAGS " -s FULL_ES3")
    string(APPEND CMAKE_C_FLAGS " -s FULL_ES3")
endif ()


set(GLUE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Engine/Source)


set(GLUE_INCLUDE_DIRS
        ${GLUE_SOURCE_DIR}
        ${GLUE_SOURCE_DIR}/Engine
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
    list(APPEND GLUE_LINK_LIBRARIES ${OGRE_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS})
endif ()
if (SDL2_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${SDL2_LIBRARIES})
endif ()
if (pugixml_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES pugixml)
endif ()
if (Bullet_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${BULLET_LIBRARIES})
    list(APPEND GLUE_INCLUDE_DIRS ${BULLET_INCLUDE_DIR})
endif ()
if (Vorbis_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${VORBIS_LIBRARIES})
endif ()
if (Ogg_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${OGG_LIBRARIES})
endif ()
if (OpenAL_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${OPENAL_LIBRARY})
endif ()
if (Lua_FOUND)
    list(APPEND GLUE_LINK_LIBRARIES ${LUA_LIBRARY})
endif ()
list(APPEND GLUE_LINK_LIBRARIES ${SYSTEM_LIBRARIES})


file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES ${GLUE_SOURCE_DIR}/*.cpp ${GLUE_SOURCE_DIR}/*.h ${GLUE_SOURCE_DIR}/*.hpp)


# this required to ti disable windows upscaling @hdp monitors @windows
if (MINGW)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Engine/Source/manifest.rc)
elseif (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Engine/Source/app.manifest)
endif ()

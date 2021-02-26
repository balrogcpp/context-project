#MIT License
#
#Copyright (c) 2020 Andrey Vasiliev
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

cmake_minimum_required(VERSION 3.16.3)

project(xio-demo)

find_package(OGRE QUIET)
find_package(Bullet QUIET)
find_package(SDL2 QUIET)
find_package(OpenAL QUIET)
find_package(Lua QUIET)
find_package(yaml-cpp QUIET)
find_package(sol2 QUIET)
find_package(ZLIB QUIET)
find_package(Lua51 QUIET)
find_package(ZZip QUIET)
find_package(assimp QUIET)

#if (NOT OGRE_FOUND)
#    return()
#endif ()

list(REMOVE_ITEM OGRE_LIBRARIES "dl")
foreach (I IN LISTS OGRE_PLUGINS OGRE_LIBRARIES)
    string(REPLACE "Static" "" X "${I}")
    if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL Debug)
        list(APPEND OGRE_LIBRARIES_STATIC "${X}Static_d")
    else ()
        list(APPEND OGRE_LIBRARIES_STATIC "${X}Static")
    endif ()
endforeach ()

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic opengl32 imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (UNIX AND NOT ANDROID)
    list(APPEND SYSTEM_LIBRARIES z OpenGL GLX X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES z.a log android EGL GLESv2 GLESv1_CM OpenSLES dl m.a hidapi ndk_compat omp)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES ${ZLIB_LIBRARIES} opengl32 winmm Version imm32 Setupapi)
elseif (APPLE)
    list(APPEND SYSTEM_LIBRARIES ${ZLIB_LIBRARIES})
endif ()

list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIR})
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/btogre/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/oggsound/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/pgeometry/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/gorilla/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${SOL2_INCLUDE_DIRS})
list(APPEND CONTEXT_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
list(APPEND CONTEXT_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
list(APPEND CONTEXT_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS})
list(APPEND CONTEXT_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus)
list(APPEND CONTEXT_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2)
list(APPEND CONTEXT_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES)
set(CONTEXT_DEPENDENCY_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h ../sources/*.cpp ../sources/*.h)

if (ANDROID)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()

if (NOT ANDROID)
    add_executable(demo ${SOURCE_FILES})
else ()
    add_library(demo SHARED ${SOURCE_FILES})
endif ()

include_directories(demo ${CMAKE_CURRENT_SOURCE_DIR}/include ${CONTEXT_DEPENDENCY_INCLUDE_DIRS} ${ANDROID_NDK}/sources/android/native_app_glue)
target_link_directories(demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})
target_link_libraries(demo PUBLIC ${OGRE_LIBRARIES} ${BULLET_LIBRARIES} vorbisfile vorbis vorbisenc ogg pugixml freetype ${LUA_LIBRARIES} ${OPENAL_LIBRARY}
        ${SDL2_LIBRARIES} ${YAML_CPP_LIBRARIES} ${ASSIMP_LIBRARIES} ${ZZip_LIBRARIES} ${SYSTEM_LIBRARIES}
        )
target_precompile_headers(demo PUBLIC include/pcheader.h)
install(TARGETS demo
        CONFIGURATIONS Release
        RUNTIME DESTINATION .
        )

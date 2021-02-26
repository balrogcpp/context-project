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

project(lib-static)

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

if (NOT OGRE_FOUND)
    return()
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

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)

include_directories(lib-static ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})
add_library(lib-static STATIC ${SOURCE_FILES})
set_target_properties(lib-static PROPERTIES OUTPUT_NAME "xio-static")
target_precompile_headers(lib-static PUBLIC include/pcheader.h)

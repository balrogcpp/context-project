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

cmake_minimum_required(VERSION 3.15.1)

find_package(OpenMP REQUIRED)
if (NOT ANDROID)
    find_package(OpenGL REQUIRED)
endif ()

include(cotire)

project(context-lib)

set(CMAKE_MODULE_PATH
    "${CONTEXT_ROOT_DIR}/CMake"
    )

include(cotire)

set(OGGSOUND_HEADERS
    oggsound/include/LocklessQueue.h
    oggsound/include/OgreOggISound.h
    oggsound/include/OgreOggListener.h
    oggsound/include/OgreOggSoundCallback.h
    oggsound/include/OgreOggSoundFactory.h
    oggsound/include/OgreOggSound.h
    oggsound/include/OgreOggSoundManager.h
    oggsound/include/OgreOggSoundPrereqs.h
    oggsound/include/OgreOggSoundRecord.h
    oggsound/include/OgreOggStaticSound.h
    oggsound/include/OgreOggStaticWavSound.h
    oggsound/include/OgreOggStreamSound.h
    oggsound/include/OgreOggStreamWavSound.h
    )

set(BTOGRE_HEADERS
    btogre/include/BtOgreExtras.h
    btogre/include/BtOgreGP.h
    btogre/include/BtOgrePG.h
    )

set(CONTEXT_HEADERS
    include/Application.h
    include/AppState.h
    include/Camera.h
    include/Exception.h
    include/Input.h
    include/pcheader.h
    include/Render.h
    include/VerboseListener.h
    include/JsonConfigurator.h
    include/DotSceneLoaderB.h
    include/Field.h
    include/Physics.h
    include/OgreHeaders.h
    include/Sound.h
    include/Component.h
    include/Compositor.h
    include/NoCopy.h
    include/TerrainMaterialGeneratorB.h
    include/CubeMapCamera.h
    include/ReflectionCamera.h
    include/Gorilla.h
    include/Overlay.h
    include/Window.h
    include/PbrUtils.h
    include/Storage.h
    include/RtssUtils.h
    include/Terrain.h
    include/CompositorHelper.h
    include/ShadowSettings.h include/YamlConfigurator.h include/ComponentLocator.h include/XmlUtils.h)

set(HEADER_FILES_CONTEXT_LIB
    ${BTOGRE_HEADERS}
    ${OGGSOUND_HEADERS}
    ${CONTEXT_HEADERS}
    include/Entity.h)

set(BTOGRE_SOURCES
    btogre/src/BtOgre.cpp
    )

set(OGGSOUND_SOURCES
    oggsound/src/OgreOggISound.cpp
    oggsound/src/OgreOggListener.cpp
    oggsound/src/OgreOggSoundFactory.cpp
    oggsound/src/OgreOggSoundManager.cpp
    oggsound/src/OgreOggSoundRecord.cpp
    oggsound/src/OgreOggStaticSound.cpp
    oggsound/src/OgreOggStaticWavSound.cpp
    oggsound/src/OgreOggStreamSound.cpp
    oggsound/src/OgreOggStreamWavSound.cpp
    )

set(CONTEXT_SOURCES
    src/Application.cpp
    src/Render.cpp
    src/DotSceneLoaderB.cpp
    src/Physics.cpp
    src/Field.cpp
    src/pcheader.cpp
    src/Compositor.cpp
    src/TerrainMaterialGeneratorB.cpp
    src/Gorilla.cpp
    src/Overlay.cpp
    src/PbrUtils.cpp
    src/Terrain.cpp)

set(SOURCE_FILES_CONTEXT_LIB
    ${BTOGRE_SOURCES}
    ${OGGSOUND_SOURCES}
    ${CONTEXT_SOURCES}
    )

include_directories(context-lib ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})

add_library(context-lib-static STATIC ${SOURCE_FILES_CONTEXT_LIB} ${HEADER_FILES_CONTEXT_LIB})

set_target_properties(context-lib-static PROPERTIES OUTPUT_NAME "context-static")

set(CMAKE_CXX_FLAGS ${CONTEXT_CXX_FLAGS})
set(CMAKE_C_FLAGS ${CONTEXT_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${CONTEXT_EXE_LINKER_FLAGS})
set(CMAKE_STATIC_LINKER_FLAGS ${CONTEXT_STATIC_LINKER_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS ${CONTEXT_SHARED_LINKER_FLAGS})

if (NOT MSVC AND NOT ANDROID)
    set_target_properties(context-lib-static PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/pcheader.h")
    cotire(context-lib-static)
endif ()
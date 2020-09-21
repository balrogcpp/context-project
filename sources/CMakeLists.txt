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

set(CMAKE_MODULE_PATH "${CONTEXT_ROOT_DIR}/CMake")

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

set(PAGED_HEADERS
    pgeometry/include/GrassLoader.h
    pgeometry/include/PagedGeometry.h
    pgeometry/include/PropertyMaps.h
    pgeometry/include/StaticBillboardSet.h
    pgeometry/include/TreeLoader2D.h
    pgeometry/include/TreeLoader3D.h
    pgeometry/include/BatchPage.h
    pgeometry/include/BatchedGeometry.h
    )

set(CONTEXT_HEADERS
    include/Application.h
    include/AppState.h
    include/Camera.h
    include/Exception.h
    include/Input.h
    include/pcheader.h
    include/Renderer.h
    include/VerboseListener.h
    include/JsonConfigurator.h
    include/DotSceneLoaderB.h
    include/Pages.h
    include/Physics.h
    include/OgreHeaders.h
    include/Sound.h
    include/Component.h
    include/Compositor.h
    include/NoCopy.h
    include/TerrainMaterialGeneratorB.h
    include/Entity.h
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
    include/ShadowSettings.h
    include/YamlConfigurator.h
    include/ComponentLocator.h
    include/XmlUtils.h
    include/Locator.h
    include/HwCheck.h
    include/Singleton.h
    )

set(HEADER_FILES_CONTEXT_LIB
    ${BTOGRE_HEADERS}
    ${PAGED_HEADERS}
    ${OGGSOUND_HEADERS}
    ${CONTEXT_HEADERS}
    )

set(BTOGRE_SOURCES
    btogre/src/BtOgre.cpp
    )

set(PAGED_SOURCES
    pgeometry/src/GrassLoader.cpp
    pgeometry/src/PagedGeometry.cpp
    pgeometry/src/PropertyMaps.cpp
    pgeometry/src/StaticBillboardSet.cpp
    pgeometry/src/TreeLoader2D.cpp
    pgeometry/src/TreeLoader3D.cpp
    pgeometry/src/BatchPage.cpp
    pgeometry/src/BatchedGeometry.cpp
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
    src/Renderer.cpp
    src/DotSceneLoaderB.cpp
    src/Physics.cpp
    src/Pages.cpp
    src/Compositor.cpp
    src/TerrainMaterialGeneratorB.cpp
    src/Gorilla.cpp
    src/Overlay.cpp
    src/PbrUtils.cpp
    src/Terrain.cpp
    src/Sound.cpp
    )

set(SOURCE_FILES_CONTEXT_LIB
    ${BTOGRE_SOURCES}
    ${PAGED_SOURCES}
    ${OGGSOUND_SOURCES}
    ${CONTEXT_SOURCES}
    )

list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIR})
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/btogre/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/oggsound/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_THIRDPARTY_ROOT}/pgeometry/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/sol)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/bullet)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GL3Plus)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GL)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GLES2)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GLES2/GLSLES)
list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_ROOT_DIR}/android/include/)
set(CONTEXT_DEPENDENCY_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIRS} CACHE INTERNAL "")

include_directories(lib-static ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})
add_library(lib-static STATIC ${SOURCE_FILES_CONTEXT_LIB} ${HEADER_FILES_CONTEXT_LIB})
set_target_properties(lib-static PROPERTIES OUTPUT_NAME "xio-static")
target_precompile_headers(lib-static PUBLIC include/pcheader.h)
set(OGRE_HOME $ENV{OGRE_HOME})
set(OGRE_SDK $ENV{OGRE_SDK})
set(OGRE_SOURCE $ENV{OGRE_SOURCE})
set(OGRE_BUILD $ENV{OGRE_BUILD})
set(OGRE_DEPENDENCIES_DIR $ENV{OGRE_DEPENDENCIES_DIR})
set(PROGRAMFILES $ENV{PROGRAMFILES})

# construct search paths from environmental hints and
# OS specific guesses
if (WIN32)
    set(OGRE_PREFIX_GUESSES
            ${ENV_PROGRAMFILES}/OGRE
            C:/OgreSDK
            )
elseif (UNIX)
    set(OGRE_PREFIX_GUESSES
            /opt/ogre
            /opt/OGRE
            /usr/lib${LIB_SUFFIX}/ogre
            /usr/lib${LIB_SUFFIX}/OGRE
            /usr/local/lib${LIB_SUFFIX}/ogre
            /usr/local/lib${LIB_SUFFIX}/OGRE
            $ENV{HOME}/ogre
            $ENV{HOME}/OGRE
            )
    if (APPLE)
        set(OGRE_PREFIX_GUESSES
                ${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx
                ${OGRE_PREFIX_GUESSES}
                )
    endif ()
endif ()
set(OGRE_PREFIX_PATH
        ${OGRE_HOME} ${OGRE_SDK} ${ENV_OGRE_HOME} ${ENV_OGRE_SDK}
        ${OGRE_PREFIX_GUESSES}
        )


find_path(OGRE_INCLUDE_DIR NAMES OgreRoot.h PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")


if (OGRE_STATIC)
    set(OGRE_POSTFIX "Static")
else ()
    set(OGRE_POSTFIX "")
endif ()


string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
if (WIN32 AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    string(APPEND OGRE_POSTFIX "_d")
endif ()


find_library(OGRE_MAIN_LIBRARY OgreMain${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_MESHLOD_LIBRARY OgreMeshLodGenerator${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_OVERLAY_LIBRARY OgreOverlay${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_PAGING_LIBRARY OgrePaging${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_RTSS_LIBRARY OgreRTShaderSystem${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_TERRAIN_LIBRARY OgreTerrain${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_VOLUME_LIBRARY OgreVolume${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_OCTREE_LIBRARY Plugin_OctreeSceneManager${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_PFX_LIBRARY Plugin_ParticleFX${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_GL_LIBRARY RenderSystem_GL${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_GL3_LIBRARY RenderSystem_GL3Plus${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_GLES2_LIBRARY RenderSystem_GLES2${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_D3D9_LIBRARY RenderSystem_Direct3D9{OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_D3D11_LIBRARY RenderSystem_Direct3D11{OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_STBI_LIBRARY Codec_STBI${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_FREEIMAGE_LIBRARY Codec_FreeImage${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_GLS_LIBRARY OgreGLSupport${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_DOTSCENE_LIBRARY Plugin_DotScene${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_ASSIMP_LIBRARY Codec_Assimp${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_BITES_LIBRARY OgreBites${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_PROPERTY_LIBRARY OgreProperty${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_BULLET_LIBRARY OgreBullet${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_BSP_LIBRARY Plugin_BSPSceneManager${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_ZONE_LIBRARY Plugin_OctreeSceneManager${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_VULKAN_LIBRARY RenderSystem_Vulkan${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_TINY_LIBRARY RenderSystem_Tiny${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_PCZ_LIBRARY Plugin_PCZSceneManager${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")


if (OGRE_MAIN_LIBRARY AND OGRE_INCLUDE_DIR)
    set(OGRE_FOUND 1)
endif ()

macro(find_package_static PACKAGE VERBOSE)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    find_package(${PACKAGE} ${VERBOSE})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()

if (OGRE_BITES_LIBRARY)
    find_package(SDL2 QUIET)
    if (SDL2_FOUND)
        list(APPEND OGRE_LIBRARIES ${SDL2_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_BITES_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Bites)
endif ()
if (OGRE_OVERLAY_LIBRARY)
    find_package(Freetype QUIET)
    list(APPEND OGRE_LIBRARIES ${OGRE_OVERLAY_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Overlay)
    if (FREETYPE_FOUND)
        list(APPEND OGRE_LIBRARIES ${FREETYPE_LIBRARIES})
    endif ()
endif ()
if (OGRE_ASSIMP_LIBRARY)
    if (OGRE_STATIC)
        set(ZLIB_USE_STATIC_LIBS ON) # works with cmake >3.24
        find_package_static(ZLIB QUIET)
    else ()
        find_package(ZLIB QUIET)
    endif ()
    find_package(assimp QUIET)
    find_library(ASSIMP_IRRXML_LIBRARY IrrXML)
    if (ASSIMP_IRRXML_LIBRARY)
        list(APPEND ASSIMP_LIBRARIES ${ASSIMP_IRRXML_LIBRARY})
    endif ()
    list(APPEND OGRE_LIBRARIES ${ASSIMP_LIBRARIES} ${OGRE_ASSIMP_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/Assimp)
    if (ZLIB_FOUND)
        list(APPEND OGRE_LIBRARIES ${ZLIB_LIBRARIES})
    endif ()
endif ()
if (OGRE_PROPERTY_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PROPERTY_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Property)
endif ()
if (OGRE_BULLET_LIBRARY)
    find_package(Bullet QUIET)
    if (BULLET_LIBRARIES)
        list(APPEND OGRE_LIBRARIES ${BULLET_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_BULLET_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Bullet)
endif ()
if (OGRE_BSP_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_BSP_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/BSPSceneManager)
endif ()
if (OGRE_ZONE_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_ZONE_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/OctreeZone)
endif ()
if (OGRE_TINY_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_TINY_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/Tiny)
endif ()
if (OGRE_PCZ_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PCZ_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/PCZSceneManager)
endif ()
if (OGRE_STBI_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_STBI_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/STBICodec)
endif ()
if (OGRE_FREEIMAGE_LIBRARY)
    find_package(FreeImage QUIET)
    if (FREEIMAGE_FOUND)
        list(APPEND OGRE_LIBRARIES ${FREEIMAGE_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_FREEIMAGE_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/FreeImageCodec)
endif ()
if (OGRE_DOTSCENE_LIBRARY)
    find_package(pugixml QUIET)
    list(APPEND OGRE_LIBRARIES ${OGRE_DOTSCENE_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/DotScene)
    if (pugixml_FOUND)
        list(APPEND OGRE_LIBRARIES pugixml::static)
    endif ()
endif ()
if (OGRE_MESHLOD_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_MESHLOD_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/MeshLodGenerator)
endif ()
if (OGRE_TERRAIN_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_TERRAIN_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Terrain)
endif ()
if (OGRE_VOLUME_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_VOLUME_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Volume)
endif ()
if (OGRE_OCTREE_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_OCTREE_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/OctreeSceneManager)
endif ()
if (OGRE_PFX_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PFX_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/ParticleFX)
endif ()
if (OGRE_PAGING_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PAGING_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Paging)
endif ()
if (OGRE_RTSS_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_RTSS_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RTShaderSystem)
endif ()
if (OGRE_VULKAN_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_VULKAN_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/Vulkan)
endif ()
if (OGRE_GL3_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_GL3_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GL3Plus)
endif ()
if (OGRE_GL_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_GL_LIBRARY})
endif ()
if (OGRE_GLES2_LIBRARY)
    find_package(OpenGL QUIET)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/GLSLES)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/GLSL)
    list(APPEND OGRE_LIBRARIES ${OGRE_GLES2_LIBRARY})
    if (OpenGL_EGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_egl_LIBRARY})
    endif ()
endif ()
if (OGRE_D3D9_LIBRARY)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/Direct3D9)
    list(APPEND OGRE_LIBRARIES ${OGRE_D3D9_LIBRARY})
    list(APPEND OGRE_LIBRARIES d3dx9 dxguid)
endif ()
if (OGRE_D3D11_LIBRARY)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/Direct3D11)
    list(APPEND OGRE_LIBRARIES ${OGRE_D3D11_LIBRARY})
    list(APPEND OGRE_LIBRARIES d3d11 dxgi dxguid d3dcompiler)
endif ()
if (OGRE_MAIN_LIBRARY)
    if (OGRE_STATIC)
        set(ZLIB_USE_STATIC_LIBS ON) # works with cmake >3.24
        find_package_static(ZLIB QUIET)
    else ()
        find_package(ZLIB QUIET)
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_MAIN_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR})
    if (ZLIB_LIBRARIES)
        list(APPEND OGRE_LIBRARIES ${ZLIB_LIBRARIES})
    endif ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND OGRE_LIBRARIES dl)
    elseif (MINGW)
        list(APPEND OGRE_LIBRARIES imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
    elseif (MSVC)
        list(APPEND OGRE_LIBRARIES winmm Version imm32 Setupapi)
    endif ()
endif ()
if (OGRE_GLS_LIBRARY)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(X11 REQUIRED)
    endif ()
    find_package(OpenGL QUIET)
    find_package(OpenGLES2 QUIET)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/glad)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/KHR)
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GL)
    list(APPEND OGRE_LIBRARIES ${OGRE_GLS_LIBRARY})
    if (X11_FOUND)
        list(APPEND OGRE_LIBRARIES ${X11_LIBRARIES} ${X11_Xrandr_LIB})
    endif ()
    if (TARGET OpenGL::GL)
        list(APPEND OGRE_LIBRARIES OpenGL::GL)
    elseif (OPENGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_LIBRARIES})
    endif ()
    if (OGRE_USE_EGL AND OpenGL_EGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_egl_LIBRARY})
    endif ()
endif ()


mark_as_advanced(OGRE_MAIN_LIBRARY)
mark_as_advanced(OGRE_MESHLOD_LIBRARY)
mark_as_advanced(OGRE_OVERLAY_LIBRARY)
mark_as_advanced(OGRE_PAGING_LIBRARY)
mark_as_advanced(OGRE_RTSS_LIBRARY)
mark_as_advanced(OGRE_TERRAIN_LIBRARY)
mark_as_advanced(OGRE_VOLUME_LIBRARY)
mark_as_advanced(OGRE_OCTREE_LIBRARY)
mark_as_advanced(OGRE_PFX_LIBRARY)
mark_as_advanced(OGRE_GL_LIBRARY)
mark_as_advanced(OGRE_GL3_LIBRARY)
mark_as_advanced(OGRE_GLES2_LIBRARY)
mark_as_advanced(OGRE_D3D9_LIBRARY)
mark_as_advanced(OGRE_D3D11_LIBRARY)
mark_as_advanced(OGRE_STBI_LIBRARY)
mark_as_advanced(OGRE_FREEIMAGE_LIBRARY)
mark_as_advanced(OGRE_GLS_LIBRARY)
mark_as_advanced(OGRE_DOTSCENE_LIBRARY)
mark_as_advanced(OGRE_ASSIMP_LIBRARY)
mark_as_advanced(ASSIMP_IRRXML_LIBRARY)
mark_as_advanced(OGRE_BITES_LIBRARY)
mark_as_advanced(OGRE_PROPERTY_LIBRARY)
mark_as_advanced(OGRE_BULLET_LIBRARY)
mark_as_advanced(OGRE_BSP_LIBRARY)
mark_as_advanced(OGRE_ZONE_LIBRARY)
mark_as_advanced(OGRE_TINY_LIBRARY)
mark_as_advanced(OGRE_PCZ_LIBRARY)
mark_as_advanced(OGRE_VULKAN_LIBRARY)
mark_as_advanced(OGRE_INCLUDE_DIRS)

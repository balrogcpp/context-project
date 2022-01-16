# This source file is part of "glue project". Created by Andrey Vasiliev

find_path(OGRE_INCLUDE_DIRS NAMES OgreRoot.h PATH_SUFFIXES "OGRE")

if (OGRE_STATIC)
    string(APPEND OGRE_POSTFIX "Static")
endif ()
if (WIN32 AND DEBUG)
    string(APPEND OGRE_POSTFIX "_d")
endif ()

find_library(OGRE_MAIN_LIBRARY OgreMain${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_MESHLOD_LIBRARY OgreMeshLodGenerator${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_OVERLAY_LIBRARY OgreOverlay${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_PAGING_LIBRARY OgrePaging${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_RTSS_LIBRARY OgreRTShaderSystem${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_TERRAIN_LIBRARY OgreTerrain${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_VOLUME_LIBRARY OgreVolume${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_OCTREE_LIBRARY Plugin_OctreeSceneManager${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_PFX_LIBRARY Plugin_ParticleFX${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_GL_LIBRARY RenderSystem_GL${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_GL3_LIBRARY RenderSystem_GL3Plus${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_GLES2_LIBRARY RenderSystem_GLES2${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_STBI_LIBRARY Codec_STBI${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_GLS_LIBRARY OgreGLSupport${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_DOTSCENE_LIBRARY Plugin_DotScene${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_ASSIMP_LIBRARY Codec_Assimp${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")
find_library(OGRE_BITES_LIBRARY OgreBites${OGRE_POSTFIX} PATH_SUFFIXES "OGRE")

if (OGRE_MAIN_LIBRARY AND OGRE_INCLUDE_DIRS)
    set(OGRE_FOUND 1)
endif ()

if (OGRE_DOTSCENE_LIBRARY)
    find_package(pugixml QUIET)
    if (pugixml_FOUND)
        list(APPEND OGRE_LIBRARIES pugixml::static)
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_DOTSCENE_LIBRARY})
endif ()
if (OGRE_MESHLOD_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_MESHLOD_LIBRARY})
endif ()
if (OGRE_OVERLAY_LIBRARY)
    find_package(Freetype QUIET)
    if (FREETYPE_FOUND)
        list(APPEND OGRE_LIBRARIES ${FREETYPE_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_OVERLAY_LIBRARY})
endif ()
if (OGRE_PAGING_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PAGING_LIBRARY})
endif ()
if (OGRE_RTSS_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_RTSS_LIBRARY})
endif ()
if (OGRE_TERRAIN_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_TERRAIN_LIBRARY})
endif ()
if (OGRE_VOLUME_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_VOLUME_LIBRARY})
endif ()
if (OGRE_OCTREE_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_OCTREE_LIBRARY})
endif ()
if (OGRE_PFX_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PFX_LIBRARY})
endif ()
if (OGRE_GL3_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_GL3_LIBRARY})
endif ()
if (OGRE_GL_LIBRARY)
    find_package(OpenGL QUIET)
    if (OpenGL_OpenGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_GL_LIBRARY})
endif ()
if (OGRE_GLES2_LIBRARY)
    find_package(OpenGL QUIET)
    find_package(OpenGLES2 QUIET)
    if (OpenGLES2_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGLES2_LIBRARY})
    endif ()
    if (OpenGL_EGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_egl_LIBRARY})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_GLES2_LIBRARY})
endif ()
if (OGRE_STBI_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_STBI_LIBRARY})
endif ()
if (OGRE_GLS_LIBRARY)
    find_package(OpenGL QUIET)
    if (OpenGL_OpenGL_FOUND)
        list(APPEND OGRE_LIBRARIES ${OPENGL_gl_LIBRARY})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_GLS_LIBRARY})
endif ()
if (OGRE_MAIN_LIBRARY)
    find_package(ZLIB QUIET)
    if (ZLIB_LIBRARIES)
        list(APPEND OGRE_LIBRARIES ${ZLIB_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_MAIN_LIBRARY})
endif ()
if (OGRE_ASSIMP_LIBRARY)
    find_package(assimp QUIET)
    find_library(ASSIMP_IRRXML_LIBRARY IrrXML)
    if (ASSIMP_IRRXML_LIBRARY)
        list(APPEND ASSIMP_LIBRARIES ${ASSIMP_IRRXML_LIBRARY})
    endif ()
    list(APPEND OGRE_LIBRARIES ${ASSIMP_LIBRARIES} ${OGRE_ASSIMP_LIBRARY})
endif ()
if (OGRE_BITES_LIBRARY)
    find_package(SDL2)
    if (SDL2_FOUND)
        if (OGRE_STATIC)
            set(SDL2_LIBRARIES SDL2::SDL2-static)
        else ()
            set(SDL2_LIBRARIES SDL2::SDL2)
        endif ()
        list(APPEND OGRE_LIBRARIES ${SDL2_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_BITES_LIBRARY})
endif ()

set(OGRE_INCLUDE_DIR ${OGRE_INCLUDE_DIRS})
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Overlay)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GL)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GL3Plus)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/GLSLES)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/Assimp)
list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/DotScene)

# created by Andrey Vasiliev

# read OGRE env values
if (NOT OGRE_IGNORE_ENV)
    set(OGRE_HOME $ENV{OGRE_HOME})
    set(OGRE_SDK $ENV{OGRE_SDK})
    set(OGRE_SOURCE $ENV{OGRE_SOURCE})
    set(OGRE_BUILD $ENV{OGRE_BUILD})
    set(OGRE_DEPENDENCIES_DIR $ENV{OGRE_DEPENDENCIES_DIR})
    set(PROGRAMFILES $ENV{PROGRAMFILES})
    set(PROGRAMFILESX86 "$ENV{ProgramFiles\(x86\)}")
endif ()


# construct search paths from environmental hints and
# OS specific guesses
if (WIN32)
    set(OGRE_PREFIX_GUESSES
            ${PROGRAMFILES}/OGRE
            ${PROGRAMFILESX86}/OGRE
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
if (EXISTS ${OGRE_INCLUDE_DIR}/OgreBuildSettings.h)
    file(READ ${OGRE_INCLUDE_DIR}/OgreBuildSettings.h OGRE_BUILD_SETTINGS)
    string(REGEX MATCH "#define OGRE_VERSION_MAJOR ([0-9/.]*)" _ "${OGRE_BUILD_SETTINGS}")
    set(OGRE_VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define OGRE_VERSION_MINOR ([0-9/.]*)" _ "${OGRE_BUILD_SETTINGS}")
    set(OGRE_VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define OGRE_VERSION_PATCH ([0-9/.]*)" _ "${OGRE_BUILD_SETTINGS}")
    set(OGRE_VERSION_PATCH ${CMAKE_MATCH_1})
    set(OGRE_VERSION "${OGRE_VERSION_MAJOR}.${OGRE_VERSION_MINOR}.${OGRE_VERSION_PATCH}")
endif ()

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
find_library(OGRE_D3D9_LIBRARY RenderSystem_Direct3D9${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
find_library(OGRE_D3D11_LIBRARY RenderSystem_Direct3D11${OGRE_POSTFIX} PATHS ${OGRE_PREFIX_PATH} PATH_SUFFIXES "OGRE")
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


# Handle the QUIETLY and REQUIRED arguments and set THEORA_FOUND
# to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
set(FPHSA_NAME_MISMATCHED TRUE)
find_package_handle_standard_args(OGRE 
    REQUIRED_VARS OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY
    VERSION_VAR OGRE_VERSION
    )


macro(find_package_static package)
    if (OGRE_STATIC)
        set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    endif ()
    find_package(${package})
    if (OGRE_STATIC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
        unset(_OLD_FIND_SUFFIXES)
    endif ()
endmacro()

if (OGRE_BITES_LIBRARY)
    find_package_static(SDL2)
    if (SDL2_FOUND)
        list(APPEND OGRE_LIBRARIES ${SDL2_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_BITES_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Bites)
endif ()
if (OGRE_OVERLAY_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_OVERLAY_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Overlay)
    if (OGRE_VERSION VERSION_LESS 13.0.0)
        find_package_static(Freetype)
        if (FREETYPE_FOUND)
            list(APPEND OGRE_LIBRARIES ${FREETYPE_LIBRARIES})
        endif ()
    endif ()
endif ()
if (OGRE_ASSIMP_LIBRARY)
    find_package_static(assimp)
    find_library(ASSIMP_IRRXML_LIBRARY IrrXML)
    if (ASSIMP_IRRXML_LIBRARY)
        list(APPEND ASSIMP_LIBRARIES ${ASSIMP_IRRXML_LIBRARY})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_ASSIMP_LIBRARY} ${ASSIMP_LIBRARIES})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/Assimp)
    find_package_static(ZLIB)
    if (ZLIB_FOUND)
        list(APPEND OGRE_LIBRARIES ${ZLIB_LIBRARIES})
    endif ()
endif ()
if (OGRE_PROPERTY_LIBRARY)
    list(APPEND OGRE_LIBRARIES ${OGRE_PROPERTY_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Property)
endif ()
if (OGRE_BULLET_LIBRARY)
    find_package_static(Bullet)
    if (BULLET_LIBRARIES)
        list(APPEND OGRE_LIBRARIES ${BULLET_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_BULLET_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Bullet ${BULLET_INCLUDE_DIRS})
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
    find_package_static(FreeImage)
    if (FREEIMAGE_FOUND)
        list(APPEND OGRE_LIBRARIES ${FREEIMAGE_LIBRARIES})
    endif ()
    list(APPEND OGRE_LIBRARIES ${OGRE_FREEIMAGE_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/Plugins/FreeImageCodec)
endif ()
if (OGRE_DOTSCENE_LIBRARY)
    find_package_static(pugixml)
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
    find_package(Vulkan REQUIRED)
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
    find_package(OpenGL REQUIRED)
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
    list(APPEND OGRE_LIBRARIES ${OGRE_MAIN_LIBRARY})
    list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR})
    if (OGRE_VERSION VERSION_LESS 13.0.0)
        find_package_static(ZLIB)
        if (ZLIB_FOUND)
            list(APPEND OGRE_LIBRARIES ${ZLIB_LIBRARIES})
        endif ()
    endif ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND OGRE_LIBRARIES dl)
    elseif (MINGW)
        list(APPEND OGRE_LIBRARIES imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
    endif ()
endif ()
if (OGRE_GLS_LIBRARY)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(X11 REQUIRED)
    endif ()
    find_package(OpenGL REQUIRED)
    find_package(OpenGLES2)
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

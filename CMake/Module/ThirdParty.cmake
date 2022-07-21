# This file is part of Glue Engine. Created by Andrey Vasiliev

include(Platform)
include(Make)
include(GlueCppFlags)
include(ExternalProject)

find_package(Git REQUIRED)

set(GLUE_PREFIX_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/External/${GLUE_TOOLCHAIN_FULL}_${CMAKE_BUILD_TYPE})
set(GLUE_THIRDPARTY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/External/${GLUE_TOOLCHAIN_FULL}_${CMAKE_BUILD_TYPE}/sdk CACHE PATH "")
set(GLUE_PATCH_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/Patch)
set(GLUE_EXTERNAL_DIR ${GLUE_THIRDPARTY_ROOT})
set(GLUE_EXTERNAL_INCLUDE_DIR ${GLUE_THIRDPARTY_ROOT}/include)
set(GLUE_EXTERNAL_LIB_DIR ${GLUE_THIRDPARTY_ROOT}/lib)
set(GLUE_EXTERNAL_BIN_DIR ${GLUE_THIRDPARTY_ROOT}/bin)
if (ANDROID)
    set(GLUE_CMAKE_EXTRA_FLAGS -DANDROID_NDK=${ANDROID_NDK} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_PLATFORM=${ANDROID_PLATFORM})
endif ()

set(SDL2_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_SDL2)
externalproject_add(Target_SDL2
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                    GIT_TAG release-2.0.22
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${SDL2_CHDIR} ${GIT_EXECUTABLE} reset --hard
                    PATCH_COMMAND ${SDL2_CHDIR} ${GIT_EXECUTABLE} apply ${GLUE_PATCH_LOCATION}/sdl2-2.0.20.patch
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DSDL_SHARED_ENABLED_BY_DEFAULT=OFF
                    -DLIBC=ON
                    -DSDL_LIBC=ON
                    -DRENDER_METAL=OFF
                    -DSDL_RENDER_METAL=OFF
                    -DHIDAPI=OFF
                    -DSDL_HIDAPI=OFF
                    -DSDL_HIDAPI_JOYSTICK=OFF
                    )

externalproject_add(Target_OIS
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/wgois/OIS.git
                    GIT_TAG v1.5.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DOIS_BUILD_SHARED_LIBS=OFF
                    -DOIS_BUILD_DEMOS=OFF
                    )

externalproject_add(Target_Bullet
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
                    GIT_TAG 3.24
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DCMAKE_RELWITHDEBINFO_POSTFIX=
                    -DCMAKE_DEBUG_POSTFIX=
                    -DCMAKE_RELEASE_POSTFIX=
                    -DCMAKE_MINSIZEREL_POSTFIX=
                    -DCLAMP_VELOCITIES=0
                    -DBULLET2_MULTITHREADING=ON
                    -DBUILD_BULLET2_DEMOS=OFF
                    -DBUILD_OPENGL3_DEMOS=OFF
                    -DBUILD_CPU_DEMOS=OFF
                    -DBUILD_EXTRAS=OFF
                    -DBUILD_ENET=OFF
                    -DBUILD_CLSOCKET=OFF
                    -DUSE_GRAPHICAL_BENCHMARK=OFF
                    -DBUILD_UNIT_TESTS=OFF
                    -DBUILD_SHARED_LIBS=OFF
                    -DUSE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD=OFF
                    -DINSTALL_LIBS=ON
                    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON
                    -DUSE_MSVC_SSE2=ON
                    -DUSE_MSVC_AVX=OFF
                    -DINTERNAL_CREATE_MSVC_RELATIVE_PATH_PROJECTFILES=ON
                    )

set(OPENAL_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_OpenAL)
externalproject_add(Target_OpenAL
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
                    GIT_TAG 1.22.2
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${OPENAL_CHDIR} ${GIT_EXECUTABLE} reset --hard
                    PATCH_COMMAND ${OPENAL_CHDIR} ${GIT_EXECUTABLE} apply ${GLUE_PATCH_LOCATION}/openal-1.21.1.patch
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DALSOFT_TESTS=OFF
                    -DALSOFT_UTILS=OFF
                    -DALSOFT_EXAMPLES=OFF
                    -DALSOFT_CONFIG=OFF
                    -DALSOFT_EAX=OFF
                    -DALSOFT_BACKEND_SDL2=OFF
                    -DALSOFT_BACKEND_SNDIO=OFF
                    -DALSOFT_BACKEND_WASAPI=OFF
                    -DALSOFT_BACKEND_WINMM=OFF
                    -DALSOFT_BACKEND_WAVE=OFF
                    -DLIBTYPE=SHARED
                    -DALSOFT_OSX_FRAMEWORK=OFF
                    )

externalproject_add(Target_Ogg
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_OpenAL
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/ogg.git
                    GIT_TAG v1.3.5
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    )

externalproject_add(Target_Vorbis
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_Ogg
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/vorbis.git
                    GIT_TAG v1.3.7
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DOGG_INCLUDE_DIR=${GLUE_THIRDPARTY_ROOT}/include #for Android
                    -DOGG_LIBRARY=${GLUE_THIRDPARTY_ROOT}/lib/libogg.a #for Android
                    )

externalproject_add(Target_zlib
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/madler/zlib.git
                    GIT_TAG v1.2.12
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DBUILD_SHARED_LIBS=OFF
                    TEST_COMMAND ${CMAKE_COMMAND} -E chdir ${GLUE_THIRDPARTY_ROOT}
                    ${CMAKE_COMMAND} -E remove -f lib/libz.so lib/libzlib.dll.a lib/zlib.lib bin/libzlib.dll bin/zlib.dll lib/libz.dylib
                    TEST_AFTER_INSTALL true
                    )

externalproject_add(Target_ZZIP
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_zlib
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/gdraheim/zziplib.git
                    GIT_TAG v0.13.72
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DBUILD_SHARED_LIBS=OFF
                    -DBUILD_STATIC_LIBS=ON
                    -DMSVC_STATIC_RUNTIME=OFF
                    -DZZIPMMAPPED=OFF
                    -DZZIPFSEEKO=OFF
                    -DZZIPWRAP=OFF
                    -DZZIPSDL=OFF
                    -DZZIPDOCS=OFF
                    -DZZIPTEST=OFF
                    -DZZIPBINS=OFF
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    )

externalproject_add(Target_pugixml
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/zeux/pugixml.git
                    GIT_TAG v1.12.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    )

externalproject_add(Target_PNG
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_zlib
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/glennrp/libpng.git
                    GIT_TAG v1.6.37
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DPNG_SHARED=OFF
                    -DPNG_TESTS=OFF
                    )

externalproject_add(Target_FreeType
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/freetype/freetype.git
                    GIT_TAG VER-2-11-1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON
                    -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=ON
                    -DCMAKE_DISABLE_FIND_PACKAGE_PNG=ON
                    -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON
                    -DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON
                    -DBUILD_SHARED_LIBS=OFF
                    )

externalproject_add(Target_rapidjson
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
                    GIT_TAG fcb23c2dbf561ec0798529be4f66394d3e4996d8
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DRAPIDJSON_BUILD_DOC=OFF
                    -DRAPIDJSON_BUILD_EXAMPLES=OFF
                    -DRAPIDJSON_BUILD_TESTS=OFF
                    -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=OFF
                    -DRAPIDJSON_HAS_STDSTRING=ON
                    )

set(ASSIMP_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_assimp)
externalproject_add(Target_assimp
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_zlib
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/assimp/assimp.git
                    GIT_TAG v5.2.4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${ASSIMP_CHDIR} ${GIT_EXECUTABLE} reset --hard
                    PATCH_COMMAND ${ASSIMP_CHDIR} ${GIT_EXECUTABLE} apply ${GLUE_PATCH_LOCATION}/assimp-5.1.6.patch
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DASSIMP_WARNINGS_AS_ERRORS=OFF
                    -DASSIMP_BUILD_SAMPLES=OFF
                    -DASSIMP_INSTALL_PDB=OFF
                    -DINJECT_DEBUG_POSTFIX=OFF
                    -DASSIMP_INJECT_DEBUG_POSTFIX=OFF
                    -DASSIMP_IGNORE_GIT_HASH=ON
                    -DASSIMP_BUILD_TESTS=OFF
                    -DBUILD_SHARED_LIBS=OFF
                    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
                    -DASSIMP_NO_EXPORT=ON
                    -DASSIMP_BUILD_AMF_IMPORTER=OFF
                    -DASSIMP_BUILD_3DS_IMPORTER=ON
                    -DASSIMP_BUILD_AC_IMPORTER=OFF
                    -DASSIMP_BUILD_ASE_IMPORTER=OFF
                    -DASSIMP_BUILD_ASSBIN_IMPORTER=OFF
                    -DASSIMP_BUILD_ASSXML_IMPORTER=OFF
                    -DASSIMP_BUILD_B3D_IMPORTER=OFF
                    -DASSIMP_BUILD_BVH_IMPORTER=OFF
                    -DASSIMP_BUILD_COLLADA_IMPORTER=ON
                    -DASSIMP_BUILD_DXF_IMPORTER=OFF
                    -DASSIMP_BUILD_CSM_IMPORTER=OFF
                    -DASSIMP_BUILD_HMP_IMPORTER=OFF
                    -DASSIMP_BUILD_IRRMESH_IMPORTER=OFF
                    -DASSIMP_BUILD_IRR_IMPORTER=OFF
                    -DASSIMP_BUILD_LWO_IMPORTER=OFF
                    -DASSIMP_BUILD_LWS_IMPORTER=OFF
                    -DASSIMP_BUILD_MD2_IMPORTER=OFF
                    -DASSIMP_BUILD_MD3_IMPORTER=OFF
                    -DASSIMP_BUILD_MD5_IMPORTER=OFF
                    -DASSIMP_BUILD_MDC_IMPORTER=OFF
                    -DASSIMP_BUILD_MDL_IMPORTER=OFF
                    -DASSIMP_BUILD_NFF_IMPORTER=OFF
                    -DASSIMP_BUILD_NDO_IMPORTER=OFF
                    -DASSIMP_BUILD_OFF_IMPORTER=OFF
                    -DASSIMP_BUILD_OBJ_IMPORTER=ON
                    -DASSIMP_BUILD_OGRE_IMPORTER=OFF
                    -DASSIMP_BUILD_OPENGEX_IMPORTER=ON
                    -DASSIMP_BUILD_PLY_IMPORTER=ON
                    -DASSIMP_BUILD_MS3D_IMPORTER=OFF
                    -DASSIMP_BUILD_COB_IMPORTER=OFF
                    -DASSIMP_BUILD_BLEND_IMPORTER=ON
                    -DASSIMP_BUILD_IFC_IMPORTER=OFF
                    -DASSIMP_BUILD_XGL_IMPORTER=OFF
                    -DASSIMP_BUILD_XGL_IMPORTER=OFF
                    -DASSIMP_BUILD_FBX_IMPORTER=ON
                    -DASSIMP_BUILD_Q3D_IMPORTER=OFF
                    -DASSIMP_BUILD_Q3BSP_IMPORTER=OFF
                    -DASSIMP_BUILD_RAW_IMPORTER=ON
                    -DASSIMP_BUILD_SIB_IMPORTER=OFF
                    -DASSIMP_BUILD_SMD_IMPORTER=OFF
                    -DASSIMP_BUILD_STL_IMPORTER=ON
                    -DASSIMP_BUILD_TERRAGEN_IMPORTER=OFF
                    -DASSIMP_BUILD_3D_IMPORTER=OFF
                    -DASSIMP_BUILD_X_IMPORTER=OFF
                    -DASSIMP_BUILD_X3D_IMPORTER=ON
                    -DASSIMP_BUILD_GLTF_IMPORTER=ON
                    -DASSIMP_BUILD_3MF_IMPORTER=OFF
                    -DASSIMP_BUILD_MMD_IMPORTER=OFF
                    -DASSIMP_BUILD_STEP_IMPORTER=OFF
                    )

set(OGRE_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_OGRE)
externalproject_add(Target_OGRE
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_FreeType
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre.git
                    GIT_TAG v13.4.2
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${OGRE_CHDIR} ${GIT_EXECUTABLE} reset --hard
                    PATCH_COMMAND ${OGRE_CHDIR} ${GIT_EXECUTABLE} apply ${GLUE_PATCH_LOCATION}/ogre-13.4.0.patch
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DOGRE_DEPENDENCIES_DIR=${GLUE_THIRDPARTY_ROOT}
                    -DOGRE_ENABLE_PRECOMPILED_HEADERS=ON
                    -DOGRE_BUILD_LIBS_AS_FRAMEWORKS=OFF
                    -DOGRE_ASSERT_MODE=1
                    -DOGRE_CONFIG_THREADS=3
                    -DOGRE_CONFIG_THREAD_PROVIDER=std
                    -DOGRE_CONFIG_DOUBLE=OFF
                    -DOGRE_CONFIG_NODE_INHERIT_TRANSFORM=OFF
                    -DOGRE_STATIC=ON
                    -DOGRE_NODELESS_POSITIONING=OFF
                    -DOGRE_CONFIG_ENABLE_MESHLOD=ON
                    -DOGRE_CONFIG_ENABLE_DDS=ON
                    -DOGRE_CONFIG_ENABLE_PVRTC=ON
                    -DOGRE_CONFIG_ENABLE_ETC=ON
                    -DOGRE_CONFIG_ENABLE_ASTC=ON
                    -DOGRE_CONFIG_ENABLE_QUAD_BUFFER_STEREO=OFF
                    -DOGRE_CONFIG_ENABLE_ZIP=ON
                    -DOGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE=OFF
                    -DOGRE_BUILD_COMPONENT_PYTHON=OFF
                    -DOGRE_BUILD_COMPONENT_JAVA=OFF
                    -DOGRE_BUILD_COMPONENT_CSHARP=OFF
                    -DOGRE_RESOURCEMANAGER_STRICT=2
                    -DOGRE_BUILD_RENDERSYSTEM_D3D11=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF
                    -DOGRE_BUILD_PLUGIN_CG=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_VULKAN=OFF
                    -DOGRE_BUILD_PLUGIN_GLSLANG=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_METAL=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON
                    -DOGRE_BUILD_RENDERSYSTEM_GLES2=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_GL=OFF
                    -DOGRE_CONFIG_ENABLE_GLES3_SUPPORT=ON
                    -DOGRE_CONFIG_ENABLE_GLES2_CG_SUPPORT=OFF
                    -DOGRE_CONFIG_ENABLE_GL_STATE_CACHE_SUPPORT=OFF
                    -DOGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_VULKAN=OFF
                    -DOGRE_BUILD_RENDERSYSTEM_TINY=OFF
                    -DOGRE_BUILD_COMPONENT_OVERLAY=ON
                    -DOGRE_BUILD_COMPONENT_BITES=OFF
                    -DOGRE_BUILD_COMPONENT_BULLET=OFF
                    -DOGRE_BITES_STATIC_PLUGINS=ON
                    -DOGRE_BUILD_COMPONENT_OVERLAY_IMGUI=ON
                    -DOGRE_BUILD_COMPONENT_PAGING=OFF
                    -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=ON
                    -DOGRE_BUILD_COMPONENT_PROPERTY=OFF
                    -DOGRE_BUILD_COMPONENT_VOLUME=ON
                    -DOGRE_BUILD_COMPONENT_TERRAIN=ON
                    -DOGRE_BUILD_PLUGIN_FREEIMAGE=OFF
                    -DOGRE_BUILD_PLUGIN_EXRCODEC=OFF
                    -DOGRE_BUILD_PLUGIN_ASSIMP=ON
                    -DOGRE_BUILD_PLUGIN_STBI=ON
                    -DOGRE_BUILD_PLUGIN_BSP=OFF
                    -DOGRE_BUILD_PLUGIN_PCZ=OFF
                    -DOGRE_BUILD_PLUGIN_PFX=ON
                    -DOGRE_BUILD_PLUGIN_OCTREE=ON
                    -DOGRE_BUILD_PLUGIN_DOT_SCENE=OFF
                    -DOGRE_BUILD_COMPONENT_HLMS=OFF
                    -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=ON
                    -DOGRE_BUILD_RTSHADERSYSTEM_SHADERS=ON
                    -DOGRE_BUILD_DEPENDENCIES=OFF
                    -DOGRE_BUILD_SAMPLES=OFF
                    -DOGRE_INSTALL_SAMPLES_SOURCE=OFF
                    -DOGRE_INSTALL_SAMPLES=OFF
                    -DOGRE_BUILD_TOOLS=OFF
                    -DOGRE_BUILD_XSIEXPORTER=OFF
                    -DOGRE_INSTALL_PDB=OFF
                    -DOGRE_INSTALL_DOCS=OFF
                    -DOGRE_BUILD_TESTS=OFF
                    -DOGRE_BUILD_LIBS_AS_FRAMEWORKS=OFF
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DOGRE_BUILD_PLATFORM_APPLE_IOS=OFF
                    TEST_COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${GLUE_PREFIX_LOCATION}/src/Target_OGRE/RenderSystems/GLSupport/include/GLSL
                    ${GLUE_THIRDPARTY_ROOT}/include/OGRE/RenderSystems/GLES2
                    TEST_AFTER_INSTALL true
                    )

externalproject_add(Target_FreeGLUT
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/dcnieho/FreeGLUT.git
                    GIT_TAG FG_3_2_2
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DFREEGLUT_BUILD_SHARED_LIBS=OFF
                    -DFREEGLUT_BUILD_STATIC_LIBS=ON
                    -DFREEGLUT_GLES=OFF
                    -DINSTALL_PDB=OFF
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    )

externalproject_add(Target_OGRE2
                    EXCLUDE_FROM_ALL true
                    DEPENDS Target_ZZIP Target_FreeType
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre-next.git
                    GIT_TAG v2.3.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DOGRE_STATIC=ON
                    -DOGRE_CONFIG_ENABLE_STBI=OFF
                    -DOGRE_BUILD_SAMPLES2=OFF
                    -DOGRE_INSTALL_DOCS=OFF
                    -DOGRE_BUILD_TOOLS=OFF
                    -DOGRE_CONFIG_THREADS=0
                    -DOGRE_USE_BOOST=OFF
                    -DOGRE_CONFIG_THREAD_PROVIDER=std
                    -DCMAKE_SKIP_INSTALL_RPATH=ON
                    -DCMAKE_CXX_STANDARD=11
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    )

externalproject_add(Target_Lua
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/balrogcpp/lua-cmake.git
                    GIT_TAG v5.4.4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DLUA_EXE=OFF
                    -DLUA_DOC=OFF
                    -DLUA_BUILD_WLUA=OFF
                    -DBUILD_SHARED_LIBS=OFF
                    -DLUA_USE_READLINE=OFF
                    )

externalproject_add(Target_sol2
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
                    GIT_TAG v3.3.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    )

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(JEMALLOC_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_jemalloc)
    externalproject_add(Target_jemalloc
                        EXCLUDE_FROM_ALL true
                        PREFIX ${GLUE_PREFIX_LOCATION}
                        GIT_REPOSITORY https://github.com/jemalloc/jemalloc.git
                        GIT_TAG 5.2.1
                        GIT_SHALLOW true
                        GIT_PROGRESS false
                        CONFIGURE_COMMAND ${JEMALLOC_CHDIR} ./autogen.sh
                        BUILD_COMMAND ${JEMALLOC_CHDIR} ./configure --disable-stats --prefix ${GLUE_THIRDPARTY_ROOT}
                        INSTALL_COMMAND ${JEMALLOC_CHDIR} ${MAKE_COMMAND} install_lib_static
                        )
endif ()

#std::filesystem is not supported on some platforms with c++17 support
externalproject_add(Target_filesystem
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/gulrak/filesystem.git
                    GIT_TAG v1.5.12
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DGHC_FILESYSTEM_BUILD_TESTING=OFF
                    -DGHC_FILESYSTEM_BUILD_EXAMPLES=OFF
                    )

externalproject_add(Target_GoogleTest
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    GIT_TAG release-1.11.0
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -Dgtest_force_shared_crt=ON
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    )

externalproject_add(Target_GoogleBenchmark
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/benchmark.git
                    GIT_TAG v1.6.1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DBENCHMARK_ENABLE_TESTING=OFF
                    )

externalproject_add(Target_json
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/nlohmann/json.git
                    GIT_TAG v3.10.5
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DJSON_BuildTests=OFF
                    )

externalproject_add(Target_yaml-cpp
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
                    GIT_TAG yaml-cpp-0.7.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DCMAKE_DEBUG_POSTFIX=
                    -DYAML_CPP_BUILD_TOOLS=OFF
                    -DYAML_BUILD_SHARED_LIBS=OFF
                    -DYAML_CPP_BUILD_TESTS=OFF
                    )

set(ASIO_CHDIR ${CMAKE_COMMAND} -E chdir ${GLUE_PREFIX_LOCATION}/src/Target_asio)
externalproject_add(Target_asio
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
                    GIT_TAG asio-1-22-1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E true
                    BUILD_COMMAND ${ASIO_CHDIR} ${CMAKE_COMMAND} -E copy_directory asio/include/asio ${GLUE_EXTERNAL_INCLUDE_DIR}/asio
                    INSTALL_COMMAND ${ASIO_CHDIR} ${CMAKE_COMMAND} -E copy asio/include/asio.hpp asio/include/asio ${GLUE_EXTERNAL_INCLUDE_DIR}
                    )

externalproject_add(Target_OpenSSL
                    EXCLUDE_FROM_ALL true
                    PREFIX ${GLUE_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
                    GIT_TAG 1.1.1n-20220327
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_PREFIX_PATH=${GLUE_THIRDPARTY_ROOT}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${GLUE_CMAKE_EXTRA_FLAGS}
                    -DWITH_APPS=OFF
                    )

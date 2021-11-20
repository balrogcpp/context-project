include(CMakeDependentOption)
include(ExternalProject)
find_package(Git REQUIRED)


set(CONTEXT_EXTERNAL_PREFIX_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_TOOLCHAIN})
set(CONTEXT_EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_TOOLCHAIN}/sdk CACHE PATH "")
set(CONTEXT_EXTERNAL_PATCH_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/patch)
set(CONTEXT_EXTERNAL_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
set(CONTEXT_EXTERNAL_INCLUDE_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include)
set(CONTEXT_EXTERNAL_LIB_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib CACHE PATH "")
set(CONTEXT_EXTERNAL_BIN_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin)
if (ANDROID)
    set(CONTEXT_CMAKE_EXTRA_FLAGS -DANDROID_NDK=${ANDROID_NDK} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_PLATFORM=${ANDROID_PLATFORM})
endif ()


externalproject_add(target-sdl
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                    GIT_TAG release-2.0.14
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DSDL_SHARED_ENABLED_BY_DEFAULT=false
                    -DLIBC=true
                    -DRENDER_METAL=false #cause linking error on macos
                    )


externalproject_add(target-bullet
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
                    GIT_TAG 3.17
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DCMAKE_RELWITHDEBINFO_POSTFIX=
                    -DCMAKE_DEBUG_POSTFIX=
                    -DCMAKE_MINSIZEREL_POSTFIX=
                    -DCLAMP_VELOCITIES=0
                    -DBULLET2_MULTITHREADING=true
                    -DBULLET2_USE_OPEN_MP_MULTITHREADING=false
                    -DBUILD_BULLET2_DEMOS=false
                    -DBUILD_CPU_DEMOS=false
                    -DBUILD_EXTRAS=false
                    -DBUILD_ENET=false
                    -DBUILD_CLSOCKET=false
                    -DBUILD_OPENGL3_DEMOS=false
                    -DUSE_GRAPHICAL_BENCHMARK=false
                    -DBUILD_UNIT_TESTS=false
                    -DBUILD_SHARED_LIBS=false
                    -DUSE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD=false
                    -DINSTALL_LIBS=true
                    -DUSE_MSVC_AVX=false
                    -DUSE_MSVC_FAST_FLOATINGPOINT=true
                    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=true
                    )


if (IOS OR ANDROID)
    set(OPENAL_LIBTYPE STATIC)
else ()
    set(OPENAL_LIBTYPE SHARED)
endif ()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
    set(OPENAL_CHDIR ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-openal)
    set(OPENAL_UPDATE_COMMAND ${OPENAL_CHDIR}  ${GIT_EXECUTABLE} reset --hard)
    set(OPENAL_PATCH_COMMAND ${OPENAL_CHDIR} ${GIT_EXECUTABLE} apply ${CONTEXT_EXTERNAL_PATCH_LOCATION}/openal-1.21.1-clang-mingw.patch)
endif()
externalproject_add(target-openal
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
                    GIT_TAG 1.21.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${OPENAL_UPDATE_COMMAND}
                    PATCH_COMMAND ${OPENAL_PATCH_COMMAND}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DALSOFT_TESTS=false
                    -DALSOFT_UTILS=false
                    -DALSOFT_EXAMPLES=false
                    -DALSOFT_CONFIG=false
                    -DALSOFT_BACKEND_SDL2=false
                    -DALSOFT_BACKEND_SNDIO=false
                    -DALSOFT_BACKEND_WASAPI=false
                    -DALSOFT_BACKEND_WINMM=false
                    -DALSOFT_BACKEND_WAVE=false
                    -DLIBTYPE=${OPENAL_LIBTYPE}
                    -DALSOFT_OSX_FRAMEWORK=false
                    )


externalproject_add(target-ogg
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-openal
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/ogg.git
                    GIT_TAG v1.3.5
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    )


externalproject_add(target-vorbis
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogg
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/vorbis.git
                    GIT_TAG v1.3.7
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DOGG_INCLUDE_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include #for Android
                    -DOGG_LIBRARY=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/libogg.a #for Android
                    )


externalproject_add(target-zlib
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/madler/zlib
                    GIT_TAG v1.2.11
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DBUILD_SHARED_LIBS=false
                    TEST_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        ${CMAKE_COMMAND} -E remove -f lib/libz.so lib/libzlib.dll.a lib/zlib.lib bin/libzlib.dll bin/zlib.dll lib/libz.dylib
                    TEST_AFTER_INSTALL true
                    )


externalproject_add(target-zzip
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/paroj/ZZIPlib.git
                    #GIT_TAG 4b4d9ca1134397dd6c0649fce2dfd80643e53cf6 repo is archived
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    )


externalproject_add(target-pugixml
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/zeux/pugixml.git
                    GIT_TAG v1.11.4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    )


externalproject_add(target-png
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/glennrp/libpng.git
                    GIT_TAG v1.6.37
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DPNG_SHARED=false
                    -DPNG_TESTS=false
                    )


externalproject_add(target-freetype
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-png
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/freetype/freetype.git
                    GIT_TAG VER-2-10-4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=true
                    -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=true
                    -DBUILD_SHARED_LIBS=false
                    )


externalproject_add(target-rapidjson
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
                    GIT_TAG v1.1.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DRAPIDJSON_BUILD_DOC=false
                    -DRAPIDJSON_BUILD_EXAMPLES=false
                    -DRAPIDJSON_BUILD_TESTS=false
                    -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=false
                    -DRAPIDJSON_HAS_STDSTRING=true
                    )


externalproject_add(target-json
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/nlohmann/json.git
                    GIT_TAG v3.9.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DJSON_BuildTests=false
                    )


if(NOT MSVC)
    set(ASSIMP_CHDIR ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-assimp)
    set(ASSIMP_UPDATE_COMMAND ${ASSIMP_CHDIR}  ${GIT_EXECUTABLE} reset --hard)
    set(ASSIMP_PATCH_COMMAND ${ASSIMP_CHDIR} ${GIT_EXECUTABLE} apply ${CONTEXT_EXTERNAL_PATCH_LOCATION}/assimp-5.0.1.patch)
endif()
externalproject_add(target-assimp
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib target-rapidjson
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/assimp/assimp.git
                    GIT_TAG v5.0.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${ASSIMP_UPDATE_COMMAND}
                    PATCH_COMMAND ${ASSIMP_PATCH_COMMAND}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DINJECT_DEBUG_POSTFIX=false
                    -DASSIMP_BUILD_TESTS=false
                    -DBUILD_SHARED_LIBS=false
                    -DASSIMP_BUILD_ASSIMP_TOOLS=false
                    -DASSIMP_BUILD_ZLIB=false
                    -DASSIMP_NO_EXPORT=true
                    -DASSIMP_BUILD_SAMPLES=false
                    -DASSIMP_INSTALL_PDB=false
                    -DASSIMP_BUILD_ASE_IMPORTER=false
                    -DASSIMP_BUILD_OFF_IMPORTER=false
                    -DASSIMP_BUILD_IRR_IMPORTER=false
                    -DASSIMP_BUILD_IRRMESH_IMPORTER=false
                    -DASSIMP_BUILD_OGRE_IMPORTER=false
                    -DASSIMP_BUILD_AMF_IMPORTER=false
                    -DASSIMP_BUILD_AC_IMPORTER=false
                    -DASSIMP_BUILD_ASSBIN_IMPORTER=false
                    -DASSIMP_BUILD_B3D_IMPORTER=false
                    -DASSIMP_BUILD_BVH_IMPORTER=false
                    -DASSIMP_BUILD_DXF_IMPORTER=false
                    -DASSIMP_BUILD_CSM_IMPORTER=false
                    -DASSIMP_BUILD_HMP_IMPORTER=false
                    -DASSIMP_BUILD_LWO_IMPORTER=false
                    -DASSIMP_BUILD_LWS_IMPORTER=false
                    -DASSIMP_BUILD_MD2_IMPORTER=false
                    -DASSIMP_BUILD_MD3_IMPORTER=false
                    -DASSIMP_BUILD_MD5_IMPORTER=false
                    -DASSIMP_BUILD_MDC_IMPORTER=false
                    -DASSIMP_BUILD_MDL_IMPORTER=false
                    -DASSIMP_BUILD_NFF_IMPORTER=false
                    -DASSIMP_BUILD_NDO_IMPORTER=false
                    -DASSIMP_BUILD_Q3D_IMPORTER=false
                    -DASSIMP_BUILD_Q3BSP_IMPORTER=false
                    -DASSIMP_BUILD_SIB_IMPORTER=false
                    -DASSIMP_BUILD_TERRAGEN_IMPORTER=false
                    -DASSIMP_BUILD_3MF_IMPORTER=false
                    -DASSIMP_BUILD_MMD_IMPORTER=false
                    -DASSIMP_BUILD_STEP_IMPORTER=false
                    )


# PCH on Apple cause objective-c compilation error
if (NOT APPLE)
    set(OGRE_USE_PCH true)
else()
    set(OGRE_CHDIR ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-ogre)
    set(OGRE_UPDATE_COMMAND ${OGRE_CHDIR} ${GIT_EXECUTABLE} reset --hard)
    set(OGRE_PATCH_COMMAND ${OGRE_CHDIR} ${GIT_EXECUTABLE} apply ${CONTEXT_EXTERNAL_PATCH_LOCATION}/ogre-1.12.11-apple.patch)
    set(OGRE_USE_PCH false)
endif()
externalproject_add(target-ogre
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zzip target-freetype target-pugixml target-assimp target-sdl
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre.git
                    GIT_TAG v1.12.13
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    UPDATE_COMMAND ${OGRE_UPDATE_COMMAND}
                    PATCH_COMMAND ${OGRE_PATCH_COMMAND}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_EXTRA_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_C_FLAGS=${CMAKE_EXTRA_C_FLAGS}
                    -DCMAKE_CXX_FLAGS=${CMAKE_EXTRA_CXX_FLAGS}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DOGRE_DEPENDENCIES_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DOGRE_ENABLE_PRECOMPILED_HEADERS=${OGRE_USE_PCH}
                    -DOGRE_BUILD_LIBS_AS_FRAMEWORKS=false
                    -DOGRE_ASSERT_MODE=1
                    -DOGRE_CONFIG_THREADS=3
                    -DOGRE_CONFIG_THREAD_PROVIDER=std
                    -DOGRE_STATIC=true
                    -DOGRE_NODELESS_POSITIONING=false
                    -DOGRE_BUILD_COMPONENT_PYTHON=false
                    -DOGRE_BUILD_COMPONENT_JAVA=false
                    -DOGRE_RESOURCEMANAGER_STRICT=2
                    -DOGRE_BUILD_COMPONENT_PYTHON=false
                    -DOGRE_BUILD_RENDERSYSTEM_D3D11=false
                    -DOGRE_BUILD_RENDERSYSTEM_D3D9=false
                    -DOGRE_BUILD_PLUGIN_CG=false
                    -DOGRE_BUILD_PLUGIN_GLSLANG=false
                    -DOGRE_BUILD_RENDERSYSTEM_METAL=false
                    -DOGRE_BUILD_RENDERSYSTEM_GL=false
                    -DOGRE_BUILD_RENDERSYSTEM_TINY=false
                    -DOGRE_BUILD_COMPONENT_OVERLAY=true
                    -DOGRE_BUILD_COMPONENT_BITES=false
                    -DOGRE_BUILD_COMPONENT_OVERLAY_IMGUI=true
                    -DOGRE_BUILD_COMPONENT_PAGING=false
                    -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=true
                    -DOGRE_BUILD_COMPONENT_PROPERTY=false
                    -DOGRE_BUILD_COMPONENT_VOLUME=true
                    -DOGRE_BUILD_COMPONENT_TERRAIN=true
                    -DOGRE_BUILD_PLUGIN_FREEIMAGE=false
                    -DOGRE_BUILD_PLUGIN_EXRCODEC=false
                    -DOGRE_BUILD_PLUGIN_ASSIMP=true
                    -DOGRE_BUILD_PLUGIN_STBI=true
                    -DOGRE_BUILD_PLUGIN_BSP=false
                    -DOGRE_BUILD_PLUGIN_PCZ=false
                    -DOGRE_BUILD_PLUGIN_PFX=true
                    -DOGRE_BUILD_PLUGIN_OCTREE=true
                    -DOGRE_BUILD_PLUGIN_DOT_SCENE=false
                    -DOGRE_BUILD_COMPONENT_HLMS=false
                    -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=true
                    -DOGRE_BUILD_DEPENDENCIES=false
                    -DOGRE_BUILD_SAMPLES=false
                    -DOGRE_INSTALL_SAMPLES_SOURCE=false
                    -DOGRE_INSTALL_SAMPLES=false
                    -DOGRE_BUILD_TOOLS=false
                    -DOGRE_INSTALL_PDB=false
                    -DOGRE_INSTALL_DOCS=false
                    -DOGRE_BUILD_LIBS_AS_FRAMEWORKS=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DOGRE_BUILD_PLATFORM_APPLE_IOS=false
                    TEST_COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-ogre/RenderSystems/GLSupport/include/GLSL
                        ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GLES2
                    TEST_AFTER_INSTALL true
                    )


externalproject_add(target-lua
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/balrogcpp/lua-cmake.git
                    GIT_TAG 5.4.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DLUA_EXE=false
                    -DLUA_DOC=false
                    -DLUA_BUILD_WLUA=false
                    -DBUILD_SHARED_LIBS=false
                    -DLUA_USE_READLINE=false
                    )


externalproject_add(target-sol
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-lua
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
                    GIT_TAG v3.2.3
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    )


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    externalproject_add(target-jemalloc
                        EXCLUDE_FROM_ALL true
                        PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                        GIT_REPOSITORY https://github.com/jemalloc/jemalloc.git
                        GIT_TAG 5.2.1
                        GIT_SHALLOW true
                        GIT_PROGRESS false
                        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ./autogen.sh
                        BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ./configure --disable-stats --prefix ${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ${MAKE_COMMAND} install_lib_static
                        )

    list(APPEND CONTEXT_DEPENDENCY_TARGETS target-jemalloc)
endif ()


#std::filesystem is not supported on some platforms with c++17 support
externalproject_add(target-filesystem
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/gulrak/filesystem.git
                    GIT_TAG v1.5.6
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DGHC_FILESYSTEM_BUILD_TESTING=false
                    -DGHC_FILESYSTEM_BUILD_EXAMPLES=false
                    )


externalproject_add(target-gtest
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    GIT_TAG release-1.11.0
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    )


externalproject_add(target-benchmark
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/benchmark.git
                    GIT_TAG v1.6.0
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS
                    -G "${CMAKE_GENERATOR}"
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -DBENCHMARK_ENABLE_TESTING=false
                    )


if (NOT MSVC)
    externalproject_add(target-glsl-optimizer
                        EXCLUDE_FROM_ALL true
                        PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                        GIT_REPOSITORY https://github.com/aras-p/glsl-optimizer.git
                        GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                        GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                        CMAKE_ARGS
                        -G "${CMAKE_GENERATOR}"
                        -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                        ${CONTEXT_CMAKE_EXTRA_FLAGS}
                        )
endif ()


add_custom_target(assets
        COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_ROOT_DIR} ${GIT_EXECUTABLE} clone --depth 1 https://github.com/balrogcpp/scenes.git assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CONTEXT_ROOT_DIR}/assets/.git
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CONTEXT_ROOT_DIR}/tmp
        COMMAND ${CMAKE_COMMAND} -P cmake/flat_zip.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Fetching assets..."
        )


add_custom_target(assets_ssh
        COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_ROOT_DIR} ${GIT_EXECUTABLE} clone --depth 1 git@github.com:balrogcpp/scenes.git assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CONTEXT_ROOT_DIR}/tmp
        COMMAND ${CMAKE_COMMAND} -P cmake/flat_zip.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Fetching assets..."
        )


add_custom_target(build-assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CONTEXT_ROOT_DIR}/tmp
        COMMAND ${CMAKE_COMMAND} -P cmake/flat_zip.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating assets and programs zip package..."
        )


list(APPEND CONTEXT_DEPENDENCY_TARGETS target-filesystem)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-sol)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-json)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-vorbis)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-ogre)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-bullet)

add_custom_target(dependencies DEPENDS ${CONTEXT_DEPENDENCY_TARGETS})

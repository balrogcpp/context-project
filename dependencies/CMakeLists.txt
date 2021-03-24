#MIT License
#
#Copyright (c) 2021 Andrew Vasiliev
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

project(xio-deps)

include(CMakeDependentOption)
include(ExternalProject)
find_package(Git REQUIRED)

if (ANDROID)
    find_package(OpenGLES2 REQUIRED)
else ()
    find_package(OpenGL REQUIRED)
endif ()

set(CONTEXT_MAKE make -j${CONTEXT_PROCESSOR_COUNT})
set(CONTEXT_EXTERNAL_PREFIX_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_EXTERNAL_TOOLCHAIN})
set(CONTEXT_EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_EXTERNAL_TOOLCHAIN}/sdk CACHE PATH "")
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
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DSDL_SHARED_ENABLED_BY_DEFAULT=false
                    -DDIRECTX=false
                    -DSDL_AUDIO=false
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-bullet3
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
                    GIT_TAG 3.09
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_RELWITHDEBINFO_POSTFIX=
                    -DCMAKE_DEBUG_POSTFIX=
                    -DCMAKE_MINSIZEREL_POSTFIX=
                    -DCLAMP_VELOCITIES=0
                    -DBULLET2_MULTITHREADING=true
                    -DBULLET2_USE_OPEN_MP_MULTITHREADING=true
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
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

if (APPLE OR ANDROID)
    set(OPENAL_LIBTYPE STATIC)
else ()
    set(OPENAL_LIBTYPE SHARED)
endif ()

externalproject_add(target-openal
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
                    GIT_TAG 1.21.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CONTEXT_MINGW_FIND_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DALSOFT_TESTS=false
                    -DALSOFT_UTILS=false
                    -DALSOFT_EXAMPLES=false
                    -DALSOFT_BACKEND_SDL2=false
                    -DALSOFT_BACKEND_SNDIO=false
                    -DALSOFT_BACKEND_WASAPI=false
                    -DALSOFT_BACKEND_WINMM=false
                    -DALSOFT_BACKEND_WAVE=false
                    -DLIBTYPE=${OPENAL_LIBTYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-ogg
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-openal
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/ogg.git
                    GIT_TAG v1.3.4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-vorbis
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogg
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/vorbis.git
                    GIT_TAG v1.3.6 #version 1.3.7 can't find ogg @Android
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DOGG_INCLUDE_DIRS=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include
                    -DOGG_LIBRARIES=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/libogg.a
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-zlib
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/madler/zlib
                    GIT_TAG v1.2.11
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DBUILD_SHARED_LIBS=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-zzip
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/paroj/ZZIPlib.git
                    #GIT_TAG 4b4d9ca1134397dd6c0649fce2dfd80643e53cf6 repo is archived
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-yamlcpp
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
                    GIT_TAG yaml-cpp-0.6.3
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DYAML_BUILD_SHARED_LIBS=false
                    -DYAML_CPP_BUILD_CONTRIB=false
                    -DYAML_CPP_BUILD_TESTS=false
                    -DYAML_CPP_BUILD_TOOLS=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}# -I /opt/android-ndk/sources/android/cpufeatures
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-pugixml
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/zeux/pugixml.git
                    GIT_TAG v1.11.4
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-freetype
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://git.savannah.gnu.org/git/freetype/freetype2.git
                    GIT_TAG VER-2-9-1 #2.10 compilation error
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DUNIX=true
                    -DBUILD_SHARED_LIBS=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --target install
                    )

externalproject_add(target-rapidjson
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
                    GIT_TAG v1.1.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DRAPIDJSON_BUILD_DOC=false
                    -DRAPIDJSON_BUILD_EXAMPLES=false
                    -DRAPIDJSON_BUILD_TESTS=false
                    -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=false
                    -DRAPIDJSON_HAS_STDSTRING=true
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-json
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/nlohmann/json.git
                    GIT_TAG v3.9.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    -DJSON_BuildTests=false
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-assimp
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib target-rapidjson
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/assimp/assimp.git
                    GIT_TAG v5.0.1
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DINJECT_DEBUG_POSTFIX=false
                    -DASSIMP_BUILD_TESTS=false
                    -DASSIMP_BUILD_ASSIMP_VIEW=false
                    -DBUILD_SHARED_LIBS=false
                    -DASSIMP_BUILD_ASSIMP_TOOLS=false
                    -DASSIMP_BUILD_ZLIB=false
                    -DASSIMP_NO_EXPORT=true
                    -DASSIMP_BUILD_SAMPLES=false
                    -DASSIMP_INSTALL_PDB=false
                    -DASSIMP_BUILD_IRR_IMPORTER=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-ogre
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zzip target-freetype target-sdl target-rapidjson
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre.git
                    GIT_TAG v1.12.11
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-ogre/RenderSystems/GLSupport/include/GLSL
                        ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/OGRE/RenderSystems/GLES2
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CONTEXT_MINGW_FIND_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DOGRE_DEPENDENCIES_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
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
                    -DOGRE_BUILD_RENDERSYSTEM_GL=false
                    -DOGRE_BUILD_COMPONENT_OVERLAY=true
                    -DOGRE_BUILD_COMPONENT_BITES=false
                    -DOGRE_BUILD_COMPONENT_OVERLAY_IMGUI=true
                    -DOGRE_BUILD_COMPONENT_PAGING=false
                    -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=false
                    -DOGRE_BUILD_COMPONENT_PROPERTY=false
                    -DOGRE_BUILD_COMPONENT_VOLUME=true
                    -DOGRE_BUILD_COMPONENT_TERRAIN=true
                    -DOGRE_BUILD_PLUGIN_FREEIMAGE=false
                    -DOGRE_BUILD_PLUGIN_EXRCODEC=false
                    -DOGRE_BUILD_PLUGIN_ASSIMP=false
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
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-lua
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/balrogcpp/lua-cmake.git
                    GIT_TAG 5.4.0
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DLUA_EXE=false
                    -DLUA_DOC=false
                    -DLUA_BUILD_WLUA=false
                    -DBUILD_SHARED_LIBS=false
                    -DLUA_USE_READLINE=false
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-sol
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-lua
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
                    GIT_TAG v3.2.3
                    GIT_SHALLOW true
                    GIT_PROGRESS false
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

if (ANDROID)
    externalproject_add(target-cpuinfo
                        EXCLUDE_FROM_ALL true
                        PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                        GIT_REPOSITORY https://github.com/google/cpu_features.git
                        GIT_TAG v0.6.0
                        GIT_SHALLOW true
                        GIT_PROGRESS false
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                        ${CONTEXT_CMAKE_EXTRA_FLAGS}
                        -G "${CMAKE_GENERATOR}"
                        )

    list(APPEND CONTEXT_DEPENDENCY_TARGETS target-cpuinfo)
endif ()

if (UNIX AND NOT ANDROID AND CMAKE_BUILD_TYPE STREQUAL "Release")
    externalproject_add(target-jemalloc
                        EXCLUDE_FROM_ALL true
                        PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                        GIT_REPOSITORY https://github.com/jemalloc/jemalloc.git
                        GIT_TAG 5.2.1
                        GIT_SHALLOW true
                        GIT_PROGRESS false
                        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ./autogen.sh
                        BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ./configure --disable-stats --prefix ${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                        INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-jemalloc ${CONTEXT_MAKE} install_lib_static
                        )

    list(APPEND CONTEXT_DEPENDENCY_TARGETS target-jemalloc)
endif ()

list(APPEND CONTEXT_DEPENDENCY_TARGETS target-sol)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-pugixml)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-yamlcpp)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-vorbis)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-ogre)
if (NOT (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    #Bullet has problems with openmp linking at clang_mingw combination
    list(APPEND CONTEXT_DEPENDENCY_TARGETS target-bullet3)
endif ()
add_custom_target(target-deps DEPENDS ${CONTEXT_DEPENDENCY_TARGETS})

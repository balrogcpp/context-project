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

project(xio-deps)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

include(CMakeDependentOption)
include(ExternalProject)
find_package(Git REQUIRED)

set(CONTEXT_MAKE make -j${CONTEXT_PROCESSOR_COUNT} CACHE STRING "")

set(CONTEXT_EXTERNAL_PREFIX_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_EXTERNAL_TOOLCHAIN}/ CACHE PATH "")
set(CONTEXT_EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/${CMAKE_BUILD_TYPE}/${CONTEXT_EXTERNAL_TOOLCHAIN}/sdk/ CACHE PATH "")
set(CONTEXT_EXTERNAL_PATCH_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/patch CACHE PATH "")
set(CONTEXT_EXTERNAL_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION} CACHE PATH "")
set(CONTEXT_EXTERNAL_INCLUDE_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include CACHE PATH "")
set(CONTEXT_EXTERNAL_LIB_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib CACHE PATH "")
set(CONTEXT_EXTERNAL_BIN_DIR ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin CACHE PATH "")
set(EXTERNAL_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(EXTERNAL_GIT_SHALLOW true)
set(EXTERNAL_GIT_PROGRESS false)

if (ANDROID)
    set(CONTEXT_CMAKE_EXTRA_FLAGS -DANDROID_NDK=${ANDROID_NDK} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_PLATFORM=${ANDROID_PLATFORM})
endif ()

externalproject_add(target-cpuinfo
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/cpu_features.git
                    GIT_TAG v0.4.1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-sdl
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/SDL-mirror/SDL.git
                    GIT_TAG release-2.0.12
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DSDL_SHARED_ENABLED_BY_DEFAULT=false
                    -DDIRECTX=false
                    -DSDL_AUDIO=false
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-bullet3
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
                    GIT_TAG 2.89
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=true
                    -DUSE_MSVC_INCREMENTAL_LINKING=false
                    -DUSE_MSVC_EXEPTIONS=true
                    -DUSE_MSVC_AVX=false
                    -DUSE_MSVC_FAST_FLOATINGPOINT=true
                    -DUSE_MSVC_STRING_POOLING=true
                    -DUSE_MSVC_FUNCTION_LEVEL_LINKING=true
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
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

# Build OpenAL with PulseAudio on Linux and DirectAudio on Windows
if (ANDROID)
    set(ALSOFT_LIB_TYPE STATIC)
elseif (UNIX)
    set(ALSOFT_BACKEND_PULSEAUDIO true)
    set(ALSOFT_BACKEND_ALSA false)
    set(ALSOFT_BACKEND_DAUDIO false)
    set(ALSOFT_LIB_TYPE STATIC)
elseif (MINGW)
    set(ALSOFT_BACKEND_PULSEAUDIO false)
    set(ALSOFT_BACKEND_DAUDIO true)
    set(ALSOFT_LIB_TYPE DYNAMIC)
endif ()

# Because of LGPL license of OpenAL you should link as .dll
externalproject_add(target-openal
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
                    GIT_TAG openal-soft-1.20.1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CONTEXT_MINGW_FIND_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DALSOFT_REQUIRE_PULSEAUDIO=${ALSOFT_BACKEND_PULSEAUDIO}
                    -DALSOFT_BACKEND_PULSEAUDIO=${ALSOFT_BACKEND_PULSEAUDIO}
                    -DALSOFT_REQUIRE_JACK=false
                    -DALSOFT_BACKEND_JACK=false
                    -DALSOFT_REQUIRE_COREAUDIO=false
                    -DALSOFT_BACKEND_COREAUDIO=false
                    -DALSOFT_REQUIRE_PORTAUDIO=false
                    -DALSOFT_BACKEND_PORTAUDIO=false
                    -DALSOFT_REQUIRE_SNDIO=false
                    -DALSOFT_BACKEND_SNDIO=false
                    -DALSOFT_REQUIRE_QSA=false
                    -DALSOFT_BACKEND_QSA=false
                    -DALSOFT_REQUIRE_SOLARIS=false
                    -DALSOFT_BACKEND_SOLARIS=false
                    -DALSOFT_REQUIRE_ALSA=${ALSOFT_BACKEND_ALSA}
                    -DALSOFT_BACKEND_ALSA=${ALSOFT_BACKEND_ALSA}
                    -DALSOFT_REQUIRE_OSS=false
                    -DALSOFT_BACKEND_OSS=false
                    -DALSOFT_REQUIRE_WINMM=false
                    -DALSOFT_BACKEND_WINMM=false
                    -DALSOFT_REQUIRE_WASAPI=false
                    -DALSOFT_BACKEND_WASAPI=false
                    -DALSOFT_BACKEND_WAVE=false
                    -DALSOFT_REQUIRE_DSOUND=${ALSOFT_BACKEND_DAUDIO}
                    -DALSOFT_BACKEND_DSOUND=${ALSOFT_BACKEND_DAUDIO}
                    -DLIBTYPE=${ALSOFT_LIB_TYPE}
                    -DALSOFT_TESTS=false
                    -DALSOFT_UTILS=false
                    -DALSOFT_EXAMPLES=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-ogg
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-openal
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/ogg.git
                    GIT_TAG v1.3.3
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

if (ANDROID)
    set(OGG_ANDROID_LIB libogg.a)
endif ()

externalproject_add(target-vorbis
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogg
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/vorbis.git
                    GIT_TAG v1.3.6
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DOGG_INCLUDE_DIRS=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include
                    -DOGG_LIBRARIES=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/${OGG_ANDROID_LIB}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

#Patch for MinGW static build
if (MINGW)
    set(ZLIB_STATIC_LIBRARY libzlibstatic.a)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(ZLIB_STATIC_LIBRARY libz.a)
endif ()

externalproject_add(target-zlib
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/madler/zlib
                    GIT_TAG v1.2.11
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DBUILD_SHARED_LIBS=false
                    -DSKIP_INSTALL_LIBRARIES=true
                    -DHAVE_OFF64_T=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-zlib-build
                    ${CMAKE_COMMAND} --build ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-zlib-build --target install
                    && ${CMAKE_COMMAND} -E copy ${ZLIB_STATIC_LIBRARY} ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/${ZLIB_STATIC_LIBRARY}
                    )

externalproject_add(target-freetype
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/aseprite/freetype2.git
                    GIT_TAG VER-2-9-1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DUNIX=true
                    -DBUILD_SHARED_LIBS=false
                    -DFT_WITH_PNG=false
                    -DFT_WITH_BZIP2=false
                    -DFT_WITH_ZLIB=false
                    -DFT_WITH_HARFBUZZ=false
                    -DCMAKE_DISABLE_FIND_PACKAGE_PNG=true
                    -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=true
                    -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=true
                    -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=true
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config ${EXTERNAL_BUILD_TYPE} --target install
                    ${MINGW_INSTALL_PATHC}
                    )

externalproject_add(target-zzip
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/paroj/ZZIPlib.git
                    GIT_TAG 4b4d9ca1134397dd6c0649fce2dfd80643e53cf6
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-rapidjson
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
                    GIT_TAG v1.1.0
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DRAPIDJSON_BUILD_DOC=false
                    -DRAPIDJSON_BUILD_EXAMPLES=false
                    -DRAPIDJSON_BUILD_TESTS=false
                    -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=false
                    -DRAPIDJSON_HAS_STDSTRING=true
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-yamlcpp
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
                    GIT_TAG yaml-cpp-0.6.3
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DYAML_BUILD_SHARED_LIBS=false
                    -DYAML_CPP_BUILD_CONTRIB=false
                    -DYAML_CPP_BUILD_TESTS=false
                    -DYAML_CPP_BUILD_TOOLS=false
                    -DMSVC_SHARED_RT=false
                    -DMSVC_STHREADED_RT=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS} -I /opt/android-ndk/sources/android/cpufeatures
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-pugixml
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/zeux/pugixml.git
                    GIT_TAG v1.10
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

cmake_dependent_option(CONTEXT_OGRE_BUILD_GLES2 "CONTEXT_OGRE_BUILD_GLES2" true "ANDROID;UNIX" false)
cmake_dependent_option(CONTEXT_OGRE_BUILD_GL3PLUS "CONTEXT_OGRE_BUILD_GL3PLUS" true "NOT ANDROID" false)
option(CONTEXT_OGRE_BUILD_GL "CONTEXT_OGRE_BUILD_GL" false)
option(CONTEXT_OGRE_BUILD_OCTREE "CONTEXT_OGRE_BUILD_OCTREE" true)
option(CONTEXT_OGRE_BUILD_PAGING "CONTEXT_OGRE_BUILD_PAGING" true)
option(CONTEXT_OGRE_BUILD_STBI "CONTEXT_OGRE_BUILD_STBI" true)
option(CONTEXT_OGRE_BUILD_HLMS "CONTEXT_OGRE_BUILD_HLMS" false)
option(CONTEXT_OGRE_BUILD_RTSS "CONTEXT_OGRE_BUILD_RTSS" true)
option(CONTEXT_OGRE_BUILD_TERRAIN "CONTEXT_OGRE_BUILD_TERRAIN" true)
option(CONTEXT_OGRE_BUILD_PROPERTY "CONTEXT_OGRE_BUILD_PROPERTY" false)
option(CONTEXT_OGRE_BUILD_OVERLAY "CONTEXT_OGRE_BUILD_OVERLAY" false)
option(CONTEXT_OGRE_BUILD_OVERLAY_IMGUI "CONTEXT_OGRE_BUILD_OVERLAY_IMGUI" false)
option(CONTEXT_OGRE_BUILD_PFX "CONTEXT_OGRE_BUILD_PFX" true)
option(CONTEXT_OGRE_BUILD_DOT "CONTEXT_OGRE_BUILD_DOT" false)
option(CONTEXT_OGRE_BUILD_DEPENDENCIES "CONTEXT_OGRE_BUILD_DEPENDENCIES" false)
option(CONTEXT_OGRE_BUILD_BSP "CONTEXT_OGRE_BUILD_BSP" false)
option(CONTEXT_OGRE_BUILD_PCZ "CONTEXT_OGRE_BUILD_PCZ" false)
option(CONTEXT_OGRE_BUILD_FREEIMAGE "CONTEXT_OGRE_BUILD_FREEIMAGE" false)
option(CONTEXT_OGRE_BUILD_EXRCODEC "CONTEXT_OGRE_BUILD_EXRCODEC" false)
option(CONTEXT_OGRE_BUILD_VOLUME "CONTEXT_OGRE_BUILD_VOLUME" false)
option(CONTEXT_OGRE_BUILD_MESHLODGENERATOR "CONTEXT_OGRE_BUILD_MESHLODGENERATOR" false)
option(CONTEXT_OGRE_BUILD_ASSIMP "CONTEXT_OGRE_BUILD_ASSIMP" false)

externalproject_add(target-ogre1
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib target-zzip target-freetype target-pugixml target-sdl
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre.git
                    GIT_TAG v1.12.8
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_FIND_ROOT_PATH=${CONTEXT_MINGW_FIND_ROOT_PATH}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DOGRE_CONFIG_THREADS=3
                    -DOGRE_STATIC=true
                    -DOGRE_NODELESS_POSITIONING=false
                    -DOGRE_CONFIG_THREAD_PROVIDER=std
                    -DOGRE_BUILD_COMPONENT_PYTHON=false
                    -DOGRE_BUILD_COMPONENT_JAVA=false
                    -DOGRE_RESOURCEMANAGER_STRICT=1
                    -DOGRE_BUILD_COMPONENT_PYTHON=false
                    -DOGRE_BUILD_COMPONENT_BITES=false
                    -DOGRE_BUILD_RENDERSYSTEM_D3D11=false
                    -DOGRE_BUILD_RENDERSYSTEM_D3D9=false
                    -DOGRE_BUILD_RENDERSYSTEM_GL=${CONTEXT_OGRE_BUILD_GL}
                    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=${CONTEXT_OGRE_BUILD_GL3PLUS}
                    -DOGRE_BUILD_RENDERSYSTEM_GLES2=${CONTEXT_OGRE_BUILD_GLES2}
                    -DOGRE_BUILD_COMPONENT_OVERLAY=${CONTEXT_OGRE_BUILD_OVERLAY}
                    -DOGRE_BUILD_COMPONENT_OVERLAY_IMGUI=${CONTEXT_OGRE_BUILD_OVERLAY_IMGUI}
                    -DOGRE_BUILD_COMPONENT_PAGING=${CONTEXT_OGRE_BUILD_PAGING}
                    -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=${CONTEXT_OGRE_BUILD_MESHLODGENERATOR}
                    -DOGRE_BUILD_COMPONENT_PROPERTY=${CONTEXT_OGRE_BUILD_PROPERTY}
                    -DOGRE_BUILD_COMPONENT_VOLUME=${CONTEXT_OGRE_BUILD_VOLUME}
                    -DOGRE_BUILD_COMPONENT_TERRAIN=${CONTEXT_OGRE_BUILD_TERRAIN}
                    -DOGRE_BUILD_PLUGIN_CG=false
                    -DOGRE_BUILD_PLUGIN_FREEIMAGE=${CONTEXT_OGRE_BUILD_FREEIMAGE}
                    -DOGRE_BUILD_PLUGIN_EXRCODEC=${CONTEXT_OGRE_BUILD_EXRCODEC}
                    -DOGRE_BUILD_PLUGIN_ASSIMP=${CONTEXT_OGRE_BUILD_ASSIMP}
                    -DOGRE_BUILD_PLUGIN_STBI=${CONTEXT_OGRE_BUILD_STBI}
                    -DOGRE_BUILD_PLUGIN_BSP=${CONTEXT_OGRE_BUILD_BSP}
                    -DOGRE_BUILD_PLUGIN_PCZ=${CONTEXT_OGRE_BUILD_PCZ}
                    -DOGRE_BUILD_PLUGIN_PFX=${CONTEXT_OGRE_BUILD_PFX}
                    -DOGRE_BUILD_PLUGIN_OCTREE=${CONTEXT_OGRE_BUILD_OCTREE}
                    -DOGRE_BUILD_PLUGIN_DOT_SCENE=${CONTEXT_OGRE_BUILD_DOT}
                    -DOGRE_BUILD_COMPONENT_HLMS=${CONTEXT_OGRE_BUILD_HLMS}
                    -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=${CONTEXT_OGRE_BUILD_RTSS}
                    -DOGRE_DEPENDENCIES_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DOGRE_BUILD_DEPENDENCIES=${CONTEXT_OGRE_BUILD_DEPENDENCIES}
                    -DOGRE_BUILD_SAMPLES=false
                    -DOGRE_INSTALL_SAMPLES_SOURCE=false
                    -DOGRE_INSTALL_SAMPLES=false
                    -DOGRE_BUILD_TOOLS=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
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
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DLUA_EXE=false
                    -DLUA_DOC=false
                    -DLUA_BUILD_WLUA=false
                    -DBUILD_SHARED_LIBS=false
                    -DLUA_USE_READLINE=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-sol
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-lua
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
                    GIT_TAG v3.2.1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )
if (ANDROID)
    list(APPEND CONTEXT_DEPENDENCY_TARGETS target-cpuinfo)
endif ()
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-sdl)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-sol)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-rapidjson)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-pugixml)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-yamlcpp)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-vorbis)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-bullet3)
list(APPEND CONTEXT_DEPENDENCY_TARGETS target-ogre1)
add_custom_target(deps DEPENDS ${CONTEXT_DEPENDENCY_TARGETS})

if (WIN32)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(SUFFIX_LIB_BULLET _Debug)
    elseif (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        set(SUFFIX_LIB_BULLET _RelWithDebugInfo)
    endif ()
endif ()

set(BULLET_LIBRARIES
    BulletDynamics${SUFFIX_LIB_BULLET}
    BulletCollision${SUFFIX_LIB_BULLET}
    LinearMath${SUFFIX_LIB_BULLET}
    )

if (WIN32)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(SUFFIX_LIB_SDL d)
    endif ()
endif ()

set(SDL_LIBRARIES SDL2${SUFFIX_LIB_SDL})

set(OGG_LIBRARIES ogg)
set(LUA_LIBRARIES lua)
set(VORBISFILE_LIBRARIES vorbisfile vorbis vorbisenc)

if (WIN32)
    set(OPENAL_LIBRARY OpenAL32)
elseif (UNIX)
    set(OPENAL_LIBRARY openal)
endif ()

set(SUFFIX_LIB_OGRE Static)
if (WIN32)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        string(APPEND SUFFIX_LIB_OGRE _d)
    endif ()
endif ()

if (CONTEXT_OGRE_BUILD_STBI)
    list(APPEND OGRE_PLUGINS Codec_STBI${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_FREEIMAGE)
    list(APPEND OGRE_PLUGINS Codec_FreeImage${SUFFIX_LIB_OGRE})
    list(APPEND OGRE_PLUGINS freeimage)
endif ()
if (CONTEXT_OGRE_BUILD_GL3PLUS)
    list(APPEND OGRE_PLUGINS RenderSystem_GL3Plus${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_GL)
    list(APPEND OGRE_PLUGINS RenderSystem_GL${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_GLES2)
    list(APPEND OGRE_PLUGINS RenderSystem_GLES2${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_OCTREE)
    list(APPEND OGRE_PLUGINS Plugin_OctreeSceneManager${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_PFX)
    list(APPEND OGRE_PLUGINS Plugin_ParticleFX${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_RTSS)
    list(APPEND OGRE_LIBRARIES OgreRTShaderSystem${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_HLMS)
    list(APPEND OGRE_LIBRARIES OgreHLMS${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_TERRAIN)
    list(APPEND OGRE_LIBRARIES OgreTerrain${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_PAGING)
    list(APPEND OGRE_LIBRARIES OgrePaging${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_MESHLODGENERATOR)
    list(APPEND OGRE_LIBRARIES OgreMeshLodGenerator${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_OVERLAY)
    list(APPEND OGRE_LIBRARIES OgreOverlay${SUFFIX_LIB_OGRE})
endif ()
if (CONTEXT_OGRE_BUILD_VOLUME)
    list(APPEND OGRE_LIBRARIES OgreVolume${SUFFIX_LIB_OGRE})
endif ()
list(APPEND OGRE_LIBRARIES OgreGLSupport${SUFFIX_LIB_OGRE})
list(APPEND OGRE_LIBRARIES OgreMain${SUFFIX_LIB_OGRE})

list(APPEND STATIC_LIBRARIES pugixml)
list(APPEND STATIC_LIBRARIES yaml-cpp)
list(APPEND STATIC_LIBRARIES "zzip")
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    list(APPEND STATIC_LIBRARIES "freetyped")
else ()
    list(APPEND STATIC_LIBRARIES "freetype")
endif ()

if (MINGW)
    list(APPEND STATIC_LIBRARIES zlibstatic)
    list(APPEND STATIC_LIBRARIES opengl32)
    list(APPEND STATIC_LIBRARIES imagehlp)
    list(APPEND STATIC_LIBRARIES dinput8)
    list(APPEND STATIC_LIBRARIES dxguid)
    list(APPEND STATIC_LIBRARIES dxerr8)
    list(APPEND STATIC_LIBRARIES user32)
    list(APPEND STATIC_LIBRARIES gdi32)
    list(APPEND STATIC_LIBRARIES imm32)
    list(APPEND STATIC_LIBRARIES winmm)
    list(APPEND STATIC_LIBRARIES ole32)
    list(APPEND STATIC_LIBRARIES oleaut32)
    list(APPEND STATIC_LIBRARIES shell32)
    list(APPEND STATIC_LIBRARIES version)
    list(APPEND STATIC_LIBRARIES uuid)
    list(APPEND STATIC_LIBRARIES setupapi)
    list(APPEND STATIC_LIBRARIES hid)
elseif (UNIX AND NOT ANDROID)
    list(APPEND STATIC_LIBRARIES z)
    list(APPEND STATIC_LIBRARIES OpenGL)
    list(APPEND STATIC_LIBRARIES GLX)
    list(APPEND STATIC_LIBRARIES X11)
    list(APPEND STATIC_LIBRARIES Xrandr)
    list(APPEND STATIC_LIBRARIES dl)
elseif (ANDROID)
    list(APPEND STATIC_LIBRARIES z.a)
    list(APPEND STATIC_LIBRARIES log)
    list(APPEND STATIC_LIBRARIES android)
    list(APPEND STATIC_LIBRARIES EGL)
    list(APPEND STATIC_LIBRARIES GLESv2)
    list(APPEND STATIC_LIBRARIES GLESv1_CM)
    list(APPEND STATIC_LIBRARIES OpenSLES)
    list(APPEND STATIC_LIBRARIES dl)
    list(APPEND STATIC_LIBRARIES m.a)
    list(APPEND STATIC_LIBRARIES hidapi)
    list(APPEND STATIC_LIBRARIES ndk_compat)
    list(APPEND STATIC_LIBRARIES omp)
endif ()

set(CONTEXT_STATIC_DEPENDENCY_LIBS
    ${OGRE_PLUGINS}
    ${OGRE_LIBRARIES}
    ${VORBISFILE_LIBRARIES}
    ${OGG_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${SDL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${STATIC_LIBRARIES}
    CACHE INTERNAL "")

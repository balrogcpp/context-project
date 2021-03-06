#MIT License
#
#Copyright (c) 2021 Andrei Vasilev
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

cmake_minimum_required(VERSION 3.16.3)

project(xio)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

enable_language(C)
enable_language(CXX)
include(CheckTypeSize)
include(ProcessorCount)

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)

if (NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(SEND_ERROR "Only Release RelWithDebInfo Debug build are available")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND MSVC)
    message(SEND_ERROR "Debug build is not working with MSVC. Use RelWithDebInfo instead")
endif ()


if (PTR_SIZE EQUAL 8)
    set(CONTEXT_PLATFORM "x64" CACHE STRING "")
elseif (PTR_SIZE EQUAL 4)
    set(CONTEXT_PLATFORM "x32" CACHE STRING "")
endif ()
if (WIN32)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "windows")
elseif (ANDROID)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "android")
elseif (APPLE)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "apple")
elseif (UNIX)
    set(CONTEXT_EXTERNAL_TOOLCHAIN "linux")
endif ()

if (MINGW)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-mingw")
elseif (MSVC)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-msvc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-gcc")
else ()
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${CMAKE_CXX_COMPILER_ID}")
endif ()

set(CONTEXT_EXTERNAL_TOOLCHAIN ${CONTEXT_EXTERNAL_TOOLCHAIN} CACHE STRING "")
set(CONTEXT_EXTERNAL_PLATROFM ${CONTEXT_PLATFORM} CACHE STRING "")
set(CONTEXT_FORCE_PLATFORM "" CACHE STRING "")
set(CONTEXT_BUILD_PLATFORM ${CONTEXT_PLATFORM})

processorcount(CONTEXT_PROCESSOR_COUNT)
set(CMAKE_BUILD_PARALLEL_LEVEL ${CONTEXT_PROCESSOR_COUNT})
set(CONTEXT_MAKE make -j${CONTEXT_PROCESSOR_COUNT} CACHE STRING "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT ANDROID)
    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -Ofast -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        string(APPEND CMAKE_CXX_FLAGS " -g -O0 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O0 -DDEBUG -D_DEBUG")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()

    if (MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -msse4.1 -funroll-loops -pthread")
    string(APPEND CMAKE_C_FLAGS " -msse4.1 -funroll-loops -pthread")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -fopenmp=libgomp")
        string(APPEND CMAKE_C_FLAGS " -fopenmp=libgomp")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " -fopenmp")
        string(APPEND CMAKE_C_FLAGS " -fopenmp")
    endif ()

    string(APPEND CONTEXT_CXX_FLAGS " -DSDL_MAIN_HANDLED -DRAPIDJSON_HAS_STDSTRING")
    string(APPEND CONTEXT_C_FLAGS " -DSDL_MAIN_HANDLED -DRAPIDJSON_HAS_STDSTRING")

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_CXX_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_C_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
    else ()
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin -fuse-ld=lld")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin -fuse-ld=lld")
    endif ()
elseif (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp /D \"RAPIDJSON_HAS_STDSTRING\"")
    string(APPEND CMAKE_C_FLAGS " /arch:SSE2 /EHa /MP /GS- /openmp /D \"RAPIDJSON_HAS_STDSTRING\"")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /fp:fast /GL /D \"WINAPI_MAIN_FUNC\"")
        string(APPEND CMAKE_C_FLAGS " /O2 /fp:fast /GL /D \"WINAPI_MAIN_FUNC\"")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF /SUBSYSTEM:WINDOWS")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_C_FLAGS " /D \"DEBUG\"")
    endif ()
endif ()

if (ANDROID)
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${ANDROID_ABI}")
    set(CONTEXT_BUILD_PLATFORM ${ANDROID_ABI})
else ()
    string(APPEND CONTEXT_EXTERNAL_TOOLCHAIN "-${CONTEXT_BUILD_PLATFORM}")
endif ()

set(CONTEXT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "")
set(CONTEXT_SOURCE_DIR ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT_BUILD_PLATFORM})
set(CONTEXT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CONTEXT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/xio CACHE PATH "")
set(CONTEXT_INCLUDE_DIR ${CONTEXT_SOURCE_DIR}/include CACHE PATH "")
set(CONTEXT_THIRDPARTY_ROOT ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_THIRDPARTY_DIR ${CONTEXT_THIRDPARTY_ROOT} CACHE PATH "")

add_subdirectory(${CONTEXT_ROOT_DIR}/dependencies ${CONTEXT_WORK_DIR}/dependencies)

if (CONTEXT_ONLY_DEPS)
    return()
endif ()

if (NOT GIT_SHA1)
    execute_process(
            COMMAND git rev-parse --short=8 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

set(CONTEXT_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "")
set(CONTEXT_ZIP_NAME ContextProjectDemo-${GIT_SHA1} CACHE PATH "")
set(CMAKE_INSTALL_PREFIX ${CONTEXT_INSTALL_PATH}/${CONTEXT_ZIP_NAME})

if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(SEND_ERROR "In-source builds are not allowed!")
endif ()

set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
#add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
#add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/doc)

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

list(REMOVE_ITEM OGRE_LIBRARIES "dl")
foreach (I IN LISTS OGRE_PLUGINS OGRE_LIBRARIES)
    string(REPLACE "Static" "" X "${I}")
    if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL Debug)
        list(APPEND OGRE_LIBRARIES_STATIC "${X}Static_d")
    else ()
        list(APPEND OGRE_LIBRARIES_STATIC "${X}Static")
    endif ()
endforeach ()

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic opengl32 imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (UNIX AND NOT ANDROID)
    list(APPEND SYSTEM_LIBRARIES z OpenGL GLX X11 Xrandr dl)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES z.a log android EGL GLESv2 GLESv1_CM OpenSLES dl m.a hidapi ndk_compat omp)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES ${ZLIB_LIBRARIES} opengl32 winmm Version imm32 Setupapi)
elseif (APPLE)
    list(APPEND SYSTEM_LIBRARIES ${ZLIB_LIBRARIES})
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
list(APPEND CONTEXT_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS}/Plugins/Assimp)
set(CONTEXT_DEPENDENCY_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES demo/*.cpp demo/*.h sources/*.cpp sources/*.h)

if (ANDROID)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    list(APPEND SOURCE_FILES ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()

if (NOT ANDROID)
    add_executable(demo ${SOURCE_FILES})
else ()
    add_library(demo SHARED ${SOURCE_FILES})
endif ()

include_directories(demo demo/include ${CONTEXT_DEPENDENCY_INCLUDE_DIRS} ${ANDROID_NDK}/sources/android/native_app_glue)
target_link_directories(demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})
target_link_libraries(demo PUBLIC ${OGRE_LIBRARIES} ${BULLET_LIBRARIES} vorbisfile vorbis vorbisenc ogg pugixml freetype ${LUA_LIBRARIES} ${OPENAL_LIBRARY}
        ${SDL2_LIBRARIES} ${YAML_CPP_LIBRARIES} ${ASSIMP_LIBRARIES} ${ZZip_LIBRARIES} ${SYSTEM_LIBRARIES})
target_precompile_headers(demo PUBLIC sources/include/pcheader.h)

install(TARGETS demo
        CONFIGURATIONS Release
        RUNTIME DESTINATION .
        )

file(GLOB_RECURSE SOURCE_FILES sources/*.cpp sources/*.h)

include_directories(lib-static ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})
add_library(lib-static STATIC ${SOURCE_FILES})
set_target_properties(lib-static PROPERTIES OUTPUT_NAME "xio-static")
target_precompile_headers(lib-static PUBLIC sources/include/pcheader.h)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")

if (WIN32)
    set(EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin/OpenAL32.dll)
endif ()

add_custom_target(zip-deps
        COMMAND ${CMAKE_COMMAND} -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} -P CMake/zipdir.cmake
        WORKING_DIRECTORY ${CONTEXT_ROOT_DIR}
        COMMENT "Zipping dependencies..."
        VERBATIM
        )

install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.yaml ${EXTERNAL_DLL_GLOB} ${OTHER_DLL_GLOB}
        CONFIGURATIONS Release
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )

install(FILES ${CMAKE_SOURCE_DIR}/deploy/start.sh
        CONFIGURATIONS Release
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ
        DESTINATION .
        )

file(GLOB CONTEXT_MEDIA_FILES ${CMAKE_SOURCE_DIR}/tmp/*)
install(DIRECTORY ${CONTEXT_MEDIA_FILES}
        CONFIGURATIONS Release
        DESTINATION .
        )

if (WIN32)
    set(CONTEXT_TARGET_OS "Windows")
elseif (UNIX)
    set(CONTEXT_TARGET_OS "Linux")
elseif (ANDROID)
    set(CONTEXT_TARGET_OS "Android")
endif ()

set(CONTEXT_PACKAGE_NAME "ContextProjectDemo")
set(CONTEXT_ARTIFACT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/artifacts)
set(CONTEXT_ARTIFACT_NAME ${CONTEXT_PACKAGE_NAME}-${CONTEXT_TARGET_OS}-${CONTEXT_BUILD_PLATFORM}-${GIT_SHA1})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${CONTEXT_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})

set(CPACK_NSIS_DISPLAY_NAME "Context Demo")
set(CPACK_NSIS_PACKAGE_NAME "Context Demo")
set(CPACK_NSIS_MENU_LINKS "demo.exe" "ContextProjectDemo")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL true)

set(CPACK_GENERATOR "ZIP")

include(CPack)

add_custom_target(install-zip
        COMMAND ${CMAKE_COMMAND} -E tar cf ${CONTEXT_ZIP_NAME}.zip --format=zip ${CONTEXT_ZIP_NAME}
        WORKING_DIRECTORY ${CONTEXT_INSTALL_PATH}
        COMMENT "Create zip..."
        VERBATIM
        )

add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)

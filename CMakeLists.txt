cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
enable_language(C)
enable_language(CXX)

project(glue CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")


include(CheckTypeSize)
include(ProcessorCount)

check_type_size("void*" PTR_SIZE BUILTIN_TYPES_ONLY)


# Build type definition
if (NOT ANDROID)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif ()
    if (NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(SEND_ERROR "Only Release/RelWithDebInfo/Debug build are available")
    endif ()
    if (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(SEND_ERROR "Debug build is not working with MSVC right now")
    endif ()
endif ()


# Platform definition
if (WIN32)
    set(CONTEXT_TOOLCHAIN "windows")
elseif (ANDROID)
    set(CONTEXT_TOOLCHAIN "android")
elseif (APPLE)
    set(CONTEXT_TOOLCHAIN "apple")
    if (IOS)
        string(APPEND CONTEXT_TOOLCHAIN "-ios")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        string(APPEND CONTEXT_TOOLCHAIN "-darwin")
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CONTEXT_TOOLCHAIN "linux")
else ()
    set(CONTEXT_TOOLCHAIN ${CMAKE_SYSTEM_NAME})
endif ()


# Toolchain definition
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(APPEND CONTEXT_TOOLCHAIN "-gcc")
    if (MINGW)
        string(APPEND CONTEXT_TOOLCHAIN "-mingw")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CONTEXT_TOOLCHAIN "-clang")
    if (MINGW)
        string(APPEND CONTEXT_TOOLCHAIN "-mingw")
    endif ()
    if (MSVC)
        string(APPEND CONTEXT_TOOLCHAIN "-msvc")
    endif ()
elseif (MSVC)
    string(APPEND CONTEXT_TOOLCHAIN "-msvc")
else ()
    string(APPEND CONTEXT_TOOLCHAIN "-${CMAKE_CXX_COMPILER_ID}")
endif ()

#Processor definition
set(CONTEXT_TOOLCHAIN ${CONTEXT_TOOLCHAIN} CACHE STRING "")
set(CONTEXT_EXTERNAL_PLATROFM ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "")
processorcount(CONTEXT_PROCESSOR_COUNT)
set(CMAKE_BUILD_PARALLEL_LEVEL ${CONTEXT_PROCESSOR_COUNT})
set(MAKE_COMMAND make -j${CONTEXT_PROCESSOR_COUNT} CACHE STRING "")
string(APPEND CONTEXT_TOOLCHAIN "-${CMAKE_SYSTEM_PROCESSOR}")


# Compilation flags definition
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -s -Ofast -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -s -Ofast -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-linker-plugin")
        if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${CONTEXT_PROCESSOR_COUNT}")
        else ()
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=thin")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=thin")
        endif ()
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        string(APPEND CMAKE_CXX_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
        string(APPEND CMAKE_C_FLAGS " -g -O2 -DDEBUG -D_DEBUG")
    endif ()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        string(APPEND CMAKE_CXX_FLAGS " -msse4.1")
        string(APPEND CMAKE_C_FLAGS " -msse4.1")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(APPEND CMAKE_CXX_FLAGS " -floop-parallelize-all")
        string(APPEND CMAKE_C_FLAGS " -floop-parallelize-all")
    endif ()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows")
        string(APPEND CMAKE_C_FLAGS " -mwindows")
    endif ()

    #This flags are useful for dependencies build
    set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
    set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")

    string(APPEND CMAKE_CXX_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wall")

    if (ENABLE_WERROR)
        string(APPEND CMAKE_CXX_FLAGS " -Werror -Wno-error=switch")
        #For OGRE
        string(APPEND CMAKE_CXX_FLAGS " -Wno-error=deprecated-declarations")
        #For Bullet
        string(APPEND CMAKE_CXX_FLAGS " -Wno-error=unused-but-set-variable")

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_CXX_FLAGS " -Wno-error=unused-command-line-argument")
            string(APPEND CMAKE_CXX_FLAGS " -Wno-error=unknown-warning-option")
        endif ()
    endif ()

    # gcc-mingw links everything as shared libraries by default
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -no-pie")
    endif ()


    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -static-libstdc++ -static-libgcc")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -static-libstdc++ -static-libgcc")
    endif ()

    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    string(APPEND CMAKE_C_FLAGS " -pthread")


    if (USE_OPENMP)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_PARALLEL")
            string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_PARALLEL")

            string(APPEND CMAKE_CXX_FLAGS " -fopenmp")
            string(APPEND CMAKE_C_FLAGS " -fopenmp")
        endif ()

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -fopenmp -static-openmp")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fopenmp -static-openmp")
        endif ()
    endif ()

elseif (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)


    string(APPEND CMAKE_CXX_FLAGS " /arch:SSE2 /EHa /MP /GS- /GF /W3")
    string(APPEND CMAKE_C_FLAGS " /arch:SSE2 /EHa /MP /GS- /GF /W3")

    if (${CMAKE_BUILD_TYPE} STREQUAL Release)
        string(APPEND CMAKE_CXX_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL")
        string(APPEND CMAKE_C_FLAGS " /O2 /Ot /Ob2 /fp:fast /GL")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF /SUBSYSTEM:WINDOWS")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_C_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /SUBSYSTEM:WINDOWS")
    endif ()

    #This flags are useful for dependencies build
    set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
    set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")
endif ()

#Set as cached variables
set(CONTEXT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "")
set(CONTEXT_SOURCE_DIR ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR})
set(CONTEXT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
set(CONTEXT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/glue CACHE PATH "")
set(CONTEXT_INCLUDE_DIR ${CONTEXT_SOURCE_DIR}/include CACHE PATH "")
set(CONTEXT_THIRDPARTY_ROOT ${CONTEXT_ROOT_DIR}/sources CACHE PATH "")
set(CONTEXT_THIRDPARTY_DIR ${CONTEXT_THIRDPARTY_ROOT} CACHE PATH "")
set(CONTEXT_ARTIFACT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/artifacts CACHE PATH "")
make_directory(${CONTEXT_ARTIFACT_DIR})


add_subdirectory(${CONTEXT_ROOT_DIR}/dependencies ${CONTEXT_WORK_DIR}/dependencies)
add_subdirectory(${CONTEXT_ROOT_DIR}/doc ${CONTEXT_WORK_DIR}/doc)


set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH};${CONTEXT_EXTERNAL_INSTALL_LOCATION}")


# assume built-in pthreads on MacOS
if (APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif ()
if (NOT ANDROID AND NOT IOS)
    find_package(OpenGL REQUIRED)
else ()
    find_package(OpenGLES2 REQUIRED)
endif ()
find_package(Threads QUIET)
find_package(OpenMP QUIET)
find_package(OGRE QUIET)
find_package(Bullet QUIET)
find_package(SDL2 QUIET)
find_package(nlohmann_json QUIET)
find_package(OpenAL QUIET)
find_package(Ogg QUIET)
find_package(sol2 QUIET)
find_package(ZLIB QUIET)
find_package(Lua51 QUIET)
find_package(assimp QUIET)
find_package(PNG QUIET)
find_package(freetype QUIET)
find_library(ASSIMP_IRRXML_LIBRARY IrrXML)


macro(insert_dependency VARIABLE PACKAGE)
    if (NOT ${VARIABLE})
        message("-- Not found ${VARIABLE}. Please build ${PACKAGE} first")
        set(ANY_NOT_FOUND true)
    else ()
        message("-- Found ${PACKAGE}")
    endif ()
endmacro()


message("")

insert_dependency(OGRE_FOUND OGRE)
insert_dependency(Bullet_FOUND Bullet)
insert_dependency(sol2_FOUND SOL2)
insert_dependency(OpenAL_FOUND OpenAL)
insert_dependency(SDL2_FOUND SDL2)
insert_dependency(PNG_FOUND LibPNG)
insert_dependency(assimp_FOUND assimp)

message("")

if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(target-deps-dummy SHARED android/app/src/main/cpp/main.cpp)
    add_dependencies(target-deps-dummy dependencies)
endif ()


if (ANY_NOT_FOUND)
    return()
endif ()


list(APPEND CONTEXT_INCLUDE_DIRS ${CONTEXT_INCLUDE_DIR} ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include
        ${BULLET_INCLUDE_DIR} ${OGRE_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS}/Overlay
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2 ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES
        ${OGRE_INCLUDE_DIRS}/Plugins/Assimp)

set(SAMPLE_DIR sample)
file(GLOB_RECURSE SOURCE_FILES ${SAMPLE_DIR}/*.cpp ${SAMPLE_DIR}/*.h ${SAMPLE_DIR}/*.hpp deploy/*.manifest sources/*.cpp sources/*.h sources/*.hpp)


if (ANDROID)
    add_library(${SAMPLE_DIR} SHARED ${SOURCE_FILES})
    set_target_properties(${SAMPLE_DIR} PROPERTIES OUTPUT_NAME "main")
elseif (WIN32)
    add_executable(${SAMPLE_DIR} WIN32 ${SOURCE_FILES})
else ()
    add_executable(${SAMPLE_DIR} ${SOURCE_FILES})
endif ()

include_directories(${SAMPLE_DIR} ${SAMPLE_DIR}/include ${CONTEXT_INCLUDE_DIRS})
target_link_directories(${SAMPLE_DIR} PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})
target_precompile_headers(${SAMPLE_DIR} PUBLIC sources/include/pcheader.h)


if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #jemalloc fix problems with memory bloat on linux
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl jemalloc.a)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES SDL2::hidapi android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()

target_link_libraries(${SAMPLE_DIR} PUBLIC ${OGRE_LIBRARIES} Codec_AssimpStatic ${ASSIMP_LIBRARIES} ${ASSIMP_IRRXML_LIBRARY} ${BULLET_LIBRARIES}
        ${PNG_LIBRARY} vorbisfile vorbis vorbisenc Ogg::ogg pugixml SDL2::SDL2-static
        ${LUA_LIBRARIES} ${OPENAL_LIBRARY} ${SYSTEM_LIBRARIES})


install(TARGETS ${SAMPLE_DIR} RUNTIME DESTINATION .)

file(GLOB_RECURSE SOURCE_FILES sources/*.cpp sources/*.h sources/*.hpp)
add_library(glue STATIC ${SOURCE_FILES})
include_directories(glue ${CONTEXT_INCLUDE_DIRS})
set_target_properties(glue PROPERTIES OUTPUT_NAME "glue-static")
target_precompile_headers(glue PUBLIC sources/include/pcheader.h)

set(CONTEXT_DEPLOY_DIR ${CONTEXT_ROOT_DIR}/deploy CACHE PATH "")


if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/bin/*.dll)
    file(INSTALL ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()

if (NOT ANDROID)
    file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/config.json DESTINATION .)
endif ()
if (MINGW)
    file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/${SAMPLE_DIR}.exe.manifest DESTINATION .)
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
set(CONTEXT_ZIP_NAME GlueSample-${GIT_SHA1} CACHE PATH "")
set(CMAKE_INSTALL_PREFIX ${CONTEXT_INSTALL_PATH}/${CONTEXT_ZIP_NAME})

set(CMAKE_PREFIX_PATH ${CONTEXT_EXTERNAL_INSTALL_LOCATION})
set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH};${CONTEXT_EXTERNAL_INSTALL_LOCATION}")


install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.json ${EXTERNAL_DLL_GLOB} DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/tmp/assets ${CMAKE_SOURCE_DIR}/tmp/programs DESTINATION .)
set(EXECUTABLE_NAME ${SAMPLE_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/Launch.sh PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
endif ()
if (WIN32)
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/Launch.bat DESTINATION .)
endif ()
if (MINGW)
    #MinGW can't compile .manifest files into executable
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/${EXECUTABLE_NAME}.exe.manifest DESTINATION .)
endif ()
if (MSVC)
    # Microsoft doesn't include MSVC dll's in standart installation
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/VC_redist.x64.exe DESTINATION .)
endif ()


set(CONTEXT_PACKAGE_NAME "GlueSample")
set(CONTEXT_ARTIFACT_NAME ${CONTEXT_PACKAGE_NAME}-${CONTEXT_TOOLCHAIN}-${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND CONTEXT_ARTIFACT_NAME "-${CMAKE_BUILD_TYPE}")
endif ()

#Package block
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${CONTEXT_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${CONTEXT_ARTIFACT_NAME})


set(CPACK_GENERATOR "ZIP")

include(CPack)

if (NOT MSVC)
    add_custom_target(build-package
            COMMAND ${CMAKE_COMMAND} --build . --target package
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${CONTEXT_ARTIFACT_DIR}/_CPack_Packages
            WORKING_DIRECTORY ${CONTEXT_WORK_DIR}
            COMMENT "Build packages..."
            VERBATIM
            )
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_custom_target(docker-compose
            COMMAND GIT_HASH=${GIT_SHA1} docker-compose up --build
            COMMAND docker-compose down --volumes
            WORKING_DIRECTORY ${CONTEXT_WORK_DIR}
            COMMENT "Build with docker..."
            VERBATIM
            )
endif ()
#Tests block
add_subdirectory(${CONTEXT_ROOT_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)

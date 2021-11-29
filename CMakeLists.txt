cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(glue VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(Make)

# Build type definition
# Android use gradle
if (NOT ANDROID)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    endif ()
    if (NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(SEND_ERROR "Only Release/RelWithDebInfo/Debug builds are available")
    endif ()
endif ()

# Platform definition
if (WIN32)
    set(GLUE_TOOLCHAIN "windows")
elseif (ANDROID)
    set(GLUE_TOOLCHAIN "android")
elseif (APPLE)
    set(GLUE_TOOLCHAIN "apple")
    if (IOS)
        string(APPEND GLUE_TOOLCHAIN "-ios")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        string(APPEND GLUE_TOOLCHAIN "-darwin")
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLUE_TOOLCHAIN "linux")
else ()
    set(GLUE_TOOLCHAIN ${CMAKE_SYSTEM_NAME})
endif ()

# Toolchain definition
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(APPEND GLUE_TOOLCHAIN "-gcc")
    if (MINGW)
        string(APPEND GLUE_TOOLCHAIN "-mingw")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND GLUE_TOOLCHAIN "-clang")
    if (MINGW)
        string(APPEND GLUE_TOOLCHAIN "-mingw")
    endif ()
    if (MSVC)
        string(APPEND GLUE_TOOLCHAIN "-msvc")
    endif ()
elseif (MSVC)
    string(APPEND GLUE_TOOLCHAIN "-msvc")
else ()
    string(APPEND GLUE_TOOLCHAIN "-${CMAKE_CXX_COMPILER_ID}")
endif ()
string(APPEND GLUE_TOOLCHAIN "-${CMAKE_SYSTEM_PROCESSOR}")

# Compilation flags definition
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_CXX_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wall")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        string(APPEND CMAKE_CXX_FLAGS " -s -Ofast -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_C_FLAGS " -s -Ofast -funroll-loops -DNDEBUG")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-linker-plugin")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-linker-plugin")
        if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            string(APPEND CMAKE_EXE_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -flto=${PROCESSOR_COUNT}")
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

    option(GLUE_ENABLE_WERROR "" OFF)
    if (GLUE_ENABLE_WERROR)
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

    option(GLUE_ENABLE_OPENMP "" OFF)
    if (GLUE_ENABLE_OPENMP)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(APPEND CMAKE_C_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
            string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_PARALLEL -fopenmp")
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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
        string(APPEND CMAKE_EXE_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " /LTCG /OPT:ICF /OPT:REF")
    else ()
        string(APPEND CMAKE_CXX_FLAGS " /D \"DEBUG\"")
        string(APPEND CMAKE_C_FLAGS " /D \"DEBUG\"")
    endif ()
endif ()
# This flags are useful for thirdparty build
set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")

# Create artifacts directory
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/artifacts)
make_directory(${GLUE_ARTIFACT_DIR})

# Doxygen
find_package(Doxygen QUIET COMPONENTS dot)
find_package(LATEX QUIET COMPONENTS PDFLATEX)
if (${DOXYGEN_FOUND})
    add_custom_target(doxygen
            COMMAND ${DOXYGEN_EXECUTABLE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            VERBATIM
            )
else ()
    message("Doxygen not found. Doxygen targets are not available")
endif ()
if (UNIX)
    set(DOXYGEN_MAKE_COMMAND ${MAKE_COMMAND})
elseif (WIN32)
    set(DOXYGEN_MAKE_COMMAND "make.bat")
endif ()
if (${LATEX_PDFLATEX_FOUND})
    add_custom_target(doxygen-pdf
            DEPENDS doxygen
            COMMAND ${CMAKE_COMMAND} -E chdir ${GLUE_ARTIFACT_DIR}/Doxygen/latex ${DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${GLUE_ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${GLUE_ARTIFACT_DIR}/Doxygen
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            VERBATIM
            )
else ()
    message("Latex PDF not found. PDF generation no available")
endif ()

# Insert dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty)
set(CMAKE_PREFIX_PATH ${GLUE_EXTERNAL_INSTALL_LOCATION})
set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH};${GLUE_EXTERNAL_INSTALL_LOCATION}")
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
find_library(ASSIMP_IRRXML_LIBRARY IrrXML QUIET)

macro(insert_dependency VARIABLE PACKAGE)
    if (NOT ${VARIABLE})
        message("-- Not found ${VARIABLE}. Please build ${PACKAGE} first")
        set(ANY_NOT_FOUND true)
    else ()
        message("-- Found ${PACKAGE}")
    endif ()
endmacro()

insert_dependency(OGRE_FOUND OGRE)
insert_dependency(Bullet_FOUND Bullet)
insert_dependency(sol2_FOUND SOL2)
insert_dependency(OpenAL_FOUND OpenAL)
insert_dependency(SDL2_FOUND SDL2)
insert_dependency(PNG_FOUND LibPNG)
insert_dependency(assimp_FOUND assimp)

if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(target-deps-dummy SHARED android/app/src/main/cpp/main.cpp)
    add_dependencies(target-deps-dummy thirdparty)
endif ()
if (ANY_NOT_FOUND)
    return()
endif ()

set(GLUE_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/glue)
set(GLUE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/source/include)

list(APPEND GLUE_INCLUDE_DIRS
        ${GLUE_INCLUDE_DIR}
        ${GLUE_EXTERNAL_INSTALL_LOCATION}/include
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_INCLUDE_DIRS}/Overlay
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES
        ${OGRE_INCLUDE_DIRS}/Plugins/Assimp
        )

set(SAMPLE_EXE sample)
file(GLOB_RECURSE SAMPLE_SOURCE_FILES ${SAMPLE_EXE}/*.cpp ${SAMPLE_EXE}/*.h ${SAMPLE_EXE}/*.hpp deploy/*.manifest source/*.cpp source/*.h source/*.hpp)

if (NOT ANDROID)
    add_executable(${SAMPLE_EXE} WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(${SAMPLE_EXE} SHARED ${SAMPLE_SOURCE_FILES})
    set_target_properties(${SAMPLE_EXE} PROPERTIES OUTPUT_NAME "main")
endif ()

include_directories(${SAMPLE_EXE} ${SAMPLE_EXE}/include ${GLUE_INCLUDE_DIRS})
target_link_directories(${SAMPLE_EXE} PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
target_precompile_headers(${SAMPLE_EXE} PUBLIC source/include/pch.h)

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
if (NOT (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic)
else ()
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic_d)
endif ()
target_link_libraries(${SAMPLE_EXE} PUBLIC ${OGRE_LIBRARIES} ${OGRE_CODEC_ASSIMP} ${ASSIMP_LIBRARIES} ${ASSIMP_IRRXML_LIBRARY} ${BULLET_LIBRARIES}
        ${PNG_LIBRARY} vorbisfile vorbis vorbisenc Ogg::ogg pugixml SDL2::SDL2-static
        ${LUA_LIBRARIES} ${OPENAL_LIBRARY} ${SYSTEM_LIBRARIES}
        )

install(TARGETS ${SAMPLE_EXE} RUNTIME DESTINATION .)
file(GLOB_RECURSE SOURCE_FILES source/*.cpp source/*.h source/*.hpp)
add_library(glue STATIC ${SOURCE_FILES})
include_directories(glue ${GLUE_INCLUDE_DIRS})
set_target_properties(glue PROPERTIES OUTPUT_NAME "glue-static")
target_precompile_headers(glue PUBLIC source/include/pch.h)

# Tests
find_package(GTest QUIET)
if (GTest_FOUND)
    enable_testing()
    include(GoogleTest)

    add_executable(glue_test tests/src/TestHello.cpp)
    target_link_libraries(glue_test ${GTEST_BOTH_LIBRARIES})

    gtest_discover_tests(glue_test)
    target_link_directories(glue_test PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
else ()
    message("Google test not found. Test targets are not available")
endif ()

# Install&package block
set(GLUE_DEPLOY_DIR ${CMAKE_SOURCE_DIR}/deploy)
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${GLUE_EXTERNAL_INSTALL_LOCATION}/bin/*.dll)
    file(INSTALL ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()
if (NOT ANDROID)
    file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/config.json DESTINATION .)
endif ()
if (MINGW)
    file(INSTALL ${CMAKE_SOURCE_DIR}/deploy/${SAMPLE_EXE}.exe.manifest DESTINATION .)
endif ()

if (NOT GIT_SHA1)
    execute_process(
            COMMAND git rev-parse --short=8 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

#Packaging
set(GLUE_PACKAGE_NAME "GlueSample")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}-${GLUE_TOOLCHAIN}-${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "-${CMAKE_BUILD_TYPE}")
endif ()
if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${GLUE_ARTIFACT_NAME})
endif ()
install(FILES ${CMAKE_SOURCE_DIR}/deploy/config.json ${EXTERNAL_DLL_GLOB} DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/tmp/assets ${CMAKE_SOURCE_DIR}/tmp/programs DESTINATION .)
set(EXECUTABLE_NAME ${SAMPLE_EXE})

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

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${GLUE_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "glue-sample")
set(CPACK_PACKAGE_VENDOR "Andrew Vasiliev")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
find_package(NSIS QUIET)
if (NOT MSVC)
    set(CPACK_GENERATOR "ZIP")
elseif (${NSIS_FOUND})
    set(CPACK_GENERATOR "ZIP" "NSIS")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "GLUE_PRJ")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma \r\n SetCompressorDictSize 32")
    set(CPACK_CREATE_DESKTOP_LINKS ${SAMPLE_EXE})
    set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
    if (CMAKE_SIZEOF_VOID_P STREQUAL 8)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    endif ()
endif ()
include(CPack)

if (NOT MSVC)
    add_custom_target(build-package
            COMMAND ${CMAKE_COMMAND} --build . --target package
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Build packages..."
            VERBATIM
            )
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_custom_target(docker-compose
            COMMAND GIT_HASH=${GIT_SHA1} docker-compose up
            COMMAND docker-compose down --volumes
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building sample with docker..."
            VERBATIM
            )
    add_custom_target(docker-compose-dep
            COMMAND docker-compose -f docker-compose-dep.yml up
            COMMAND docker-compose down --volumes
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building thirdparty with docker..."
            VERBATIM
            )
endif ()

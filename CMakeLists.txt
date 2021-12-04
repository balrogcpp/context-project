cmake_minimum_required(VERSION 3.19.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(PrjGlue VERSION 0.1.0)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()
set(ALLOWED_BUILD_TYPES "Release;RelWithDebInfo;Debug")
if (NOT ${CMAKE_BUILD_TYPE} IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not valid build type. Valid are ${ALLOWED_BUILD_TYPES}")
endif ()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
include(Make)
include(Platform)
include(CppFlags)
include(GitHash)

# Build type definition
# Android use gradle
# This flags are useful for thirdparty build
set(CMAKE_EXTRA_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "")
set(CMAKE_EXTRA_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "")

# Create artifacts directory
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
make_directory(${GLUE_ARTIFACT_DIR})

# Doxygen
find_package(Doxygen COMPONENTS dot QUIET)
find_package(LATEX COMPONENTS PDFLATEX QUIET)
if (${DOXYGEN_FOUND})
    add_custom_target(Doxygen
            COMMAND ${DOXYGEN_EXECUTABLE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message("Doxygen not found. Doxygen targets are not available")
endif ()
if (UNIX)
    set(GLUE_DOXYGEN_MAKE_COMMAND ${MAKE_COMMAND})
elseif (WIN32)
    set(GLUE_DOXYGEN_MAKE_COMMAND "make.bat")
endif ()
if (${LATEX_PDFLATEX_FOUND})
    add_custom_target(DoxygenPdf
            DEPENDS Doxygen
            COMMAND ${CMAKE_COMMAND} -E chdir ${GLUE_ARTIFACT_DIR}/Doxygen/latex ${GLUE_DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${GLUE_ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${GLUE_ARTIFACT_DIR}/Doxygen
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message("Latex PDF not found. PDF generation no available")
endif ()

# Insert dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/Thirdparty)
set(CMAKE_PREFIX_PATH ${GLUE_EXTERNAL_INSTALL_LOCATION})
list(APPEND CMAKE_FIND_ROOT_PATH ${GLUE_EXTERNAL_INSTALL_LOCATION})
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

# Docker-compose target
add_custom_target(DockerComposeDep
        COMMAND docker-compose -f docker-compose-3rd.yml up
        COMMAND docker-compose -f docker-compose-3rd.yml down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building thirdparty libraries with docker-compose..."
        USES_TERMINAL
        )

# Docker-compose targets
add_custom_target(DockerCompose
        COMMAND GIT_HASH=${GIT_SHA1} docker-compose up
        COMMAND docker-compose down --volumes
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building tests with docker..."
        USES_TERMINAL
        )

if ((ANDROID OR IOS) AND ANY_NOT_FOUND)
    add_library(AndroidDummy SHARED Android/app/src/main/cpp/main.cpp)
    add_dependencies(AndroidDummy Thirdparty)
endif ()
if (ANY_NOT_FOUND)
    return()
endif ()

set(GLUE_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/glue)
set(GLUE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Source/Glue/include)

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
file(GLOB_RECURSE SAMPLE_SOURCE_FILES Source/Sample/*.cpp Source/Sample/*.h Source/Sample/*.hpp Deploy/*.manifest Source/Glue/*.cpp Source/Glue/*.h Source/Glue/*.hpp)

if (NOT ANDROID)
    add_executable(Sample WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(Sample SHARED ${SAMPLE_SOURCE_FILES})
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "main")
endif ()

include_directories(Sample Source/Sample/include ${GLUE_INCLUDE_DIRS})
target_link_directories(Sample PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
target_precompile_headers(Sample PUBLIC Source/Glue/include/pch.h)
set_target_properties(Sample PROPERTIES OUTPUT_NAME "Sample")

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
target_link_libraries(Sample PUBLIC
        ${OGRE_LIBRARIES}
        ${OGRE_CODEC_ASSIMP}
        ${ASSIMP_LIBRARIES}
        ${ASSIMP_IRRXML_LIBRARY}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        vorbisfile
        vorbis
        vorbisenc
        Ogg::ogg
        pugixml
        SDL2::SDL2-static
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

install(TARGETS Sample RUNTIME DESTINATION .)

option(GLUE_BUILD_STATIC_LIBRARY "Build static library" OFF)
if (GLUE_BUILD_STATIC_LIBRARY)
    file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES Source/*.cpp Source/*.h Source/*.hpp)
    add_library(glue STATIC ${GLUE_ENGINE_SOURCE_FILES})
    include_directories(glue ${GLUE_INCLUDE_DIRS})
    set_target_properties(glue PROPERTIES OUTPUT_NAME "glue-static")
    target_precompile_headers(glue PUBLIC Source/Glue/include/pch.h)
endif ()

# Tests
find_package(GTest QUIET)
if (GTest_FOUND AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    include(GoogleTest)

    add_executable(TestGlue Source/Tests/src/TestHello.cpp)
    target_link_libraries(TestGlue ${GTEST_BOTH_LIBRARIES})

    gtest_discover_tests(TestGlue)
    target_link_directories(TestGlue PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
else ()
    message("Google test not found. Test targets are not available.")
endif ()

# Install&package block
set(GLUE_DEPLOY_DIR ${CMAKE_SOURCE_DIR}/Deploy)
if (WIN32)
    file(GLOB EXTERNAL_DLL_GLOB ${GLUE_EXTERNAL_INSTALL_LOCATION}/bin/*.dll)
    file(INSTALL ${EXTERNAL_DLL_GLOB} DESTINATION .)
endif ()
if (NOT ANDROID)
    file(INSTALL ${CMAKE_SOURCE_DIR}/Deploy/config.json DESTINATION .)
endif ()
if (MINGW)
    file(INSTALL ${CMAKE_SOURCE_DIR}/Deploy/Sample.exe.manifest DESTINATION .)
endif ()

#Packaging
set(GLUE_PACKAGE_NAME "GlueSample")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT}_${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()
if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${GLUE_ARTIFACT_NAME})
endif ()
install(FILES ${CMAKE_SOURCE_DIR}/Deploy/config.json ${EXTERNAL_DLL_GLOB} DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Tmp/Assets ${CMAKE_SOURCE_DIR}/Tmp/Programs DESTINATION .)
set(EXECUTABLE_NAME Sample)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ OWNER_EXECUTE DESTINATION .)
endif ()
if (WIN32)
    install(FILES DESTINATION .)
endif ()
if (MINGW)
    install(FILES ${CMAKE_SOURCE_DIR}/Deploy/Sample.exe.manifest DESTINATION .)
endif ()

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${GLUE_ARTIFACT_DIR})
set(CPACK_PACKAGE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_FILE_NAME ${GLUE_ARTIFACT_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "GlueSample")
set(CPACK_PACKAGE_VENDOR "Andrew Vasiliev")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "ZIP")
find_package(NSIS QUIET)
if (${NSIS_FOUND} AND WIN32)
    list(APPEND CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "PRJ_GLUE")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma \r\n SetCompressorDictSize 32")
    set(CPACK_CREATE_DESKTOP_LINKS Sample)
    set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
    if (CMAKE_SIZEOF_VOID_P STREQUAL 8)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    endif ()
endif ()
include(CPack)

# Doesn't work with
add_custom_target(BuildPackage
        COMMAND ${CMAKE_COMMAND} --build . --target package
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Build packages..."
        USES_TERMINAL
        )

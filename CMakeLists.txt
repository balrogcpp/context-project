cmake_minimum_required(VERSION 3.19.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

# Create artifacts directory
set(GLUE_ARTIFACT_DIR ${CMAKE_SOURCE_DIR}/Artifacts)
make_directory(${GLUE_ARTIFACT_DIR})
make_directory(${GLUE_ARTIFACT_DIR}/ThirdParty)

# Insert dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty)

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
        message(STATUS "Not found ${VARIABLE}. Please build ${PACKAGE} first")
        set(ANY_NOT_FOUND true)
    else ()
        message(STATUS "Found ${PACKAGE}")
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
    add_dependencies(AndroidDummy ThirdParty)
endif ()
if (ANY_NOT_FOUND)
    return()
endif ()

# Library and Tests
add_subdirectory(${CMAKE_SOURCE_DIR}/Source)

#Packaging
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


set(GLUE_PACKAGE_NAME "GlueSample")
set(GLUE_ARTIFACT_NAME ${GLUE_PACKAGE_NAME}_${GLUE_TOOLCHAIN_SHORT}_${GIT_SHA1})
if (NOT CMAKE_BUILD_TYPE STREQUAL Release)
    string(APPEND GLUE_ARTIFACT_NAME "_${CMAKE_BUILD_TYPE}")
endif ()
if (NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Install/${GLUE_ARTIFACT_NAME})
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

# Doesn't work with MSVC
if (NOT MSVC)
    add_custom_target(BuildPackage
            COMMAND ${CMAKE_COMMAND} --build . --target package
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${GLUE_ARTIFACT_DIR}/_CPack_Packages
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Build packages..."
            USES_TERMINAL
            )
endif ()

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
    message("Doxygen not found. Doxygen targets will not be available")
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

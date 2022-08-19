# This file is part of Glue Engine. Created by Andrey Vasiliev


# GLUE_ENGINE_SOURCE_FILES, GLUE_INCLUDE_DIRS
include(GlueBuildVariables)
include(Platform)


# glob sources
file(GLOB_RECURSE SHADER_SOURCE_FILES Programs/*)


# VS project folders
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Source PREFIX "Engine" FILES ${GLUE_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/Engine/Programs PREFIX "Programs" FILES ${SHADER_SOURCE_FILES})

if (MSVC)
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES LANGUAGE Shaders)
elseif (${CMAKE_GENERATOR} STREQUAL "Xcode")
    list(APPEND GLUE_ENGINE_SOURCE_FILES ${SHADER_SOURCE_FILES})
    set_source_files_properties(${SHADER_SOURCE_FILES} PROPERTIES EXTERNAL_OBJECT TRUE)
endif ()


# Static library
add_library(GlueStatic STATIC EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
include_directories(GlueStatic ${GLUE_INCLUDE_DIRS})
set_target_properties(GlueStatic PROPERTIES OUTPUT_NAME "GlueStatic")
target_precompile_headers(GlueStatic PUBLIC Source/pch.h)


# Shared library
if (DESKTOP)
    add_library(GlueShared SHARED EXCLUDE_FROM_ALL ${GLUE_ENGINE_SOURCE_FILES})
    include_directories(GlueShared ${GLUE_INCLUDE_DIRS})
    set_target_properties(GlueShared PROPERTIES OUTPUT_NAME "GlueShared")
    target_precompile_headers(GlueShared PUBLIC Source/pch.h)
endif ()

# Documentation

include(ProcessorCount)
processorcount(PROCESSOR_COUNT)
set(MAKE_COMMAND make -j${PROCESSOR_COUNT})


# VS folder
set(CMAKE_FOLDER Doxygen)


# Doxygen generate static html site
find_package(Doxygen COMPONENTS dot QUIET)


if (${DOXYGEN_FOUND})
    message(STATUS "Doxygen found. Doxygen generation enabled")
    add_custom_target(DoxygenHtml
            COMMAND ${DOXYGEN_EXECUTABLE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message(STATUS "Doxygen not found. Doxygen generation disabled")
endif ()


# Doxygen generate pdf file with latexpdf
find_package(LATEX COMPONENTS PDFLATEX QUIET)


if (UNIX)
    set(GLUE_DOXYGEN_MAKE_COMMAND ${MAKE_COMMAND})
elseif (WIN32)
    set(GLUE_DOXYGEN_MAKE_COMMAND "make.bat")
endif ()


if (${LATEX_PDFLATEX_FOUND})
    message(STATUS "Latex PDF found. PDF generation enabled")
    add_custom_target(DoxygenLatexPdf
            DEPENDS DoxygenHtml
            COMMAND ${CMAKE_COMMAND} -E chdir ${GLUE_ARTIFACT_DIR}/Doxygen/latex ${GLUE_DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${GLUE_ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${GLUE_ARTIFACT_DIR}/Doxygen
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message(STATUS "Latex PDF not found. PDF generation disabled")
endif ()

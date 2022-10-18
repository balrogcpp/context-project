set(CMAKE_FOLDER Doxygen)
find_package(Doxygen COMPONENTS dot QUIET)
find_package(LATEX COMPONENTS PDFLATEX QUIET)
include(ProcessorCount)
processorcount(PROCESSOR_COUNT)
set(MAKE_COMMAND make -j${PROCESSOR_COUNT})

if (NOT ARTIFACT_DIR)
    message(FATAL "ARTIFACT_DIR is not defined")
endif ()

if (UNIX)
    set(DOXYGEN_MAKE_COMMAND ${MAKE_COMMAND})
elseif (WIN32)
    set(DOXYGEN_MAKE_COMMAND "make.bat")
endif ()


if (${DOXYGEN_FOUND})
    message(STATUS "Doxygen found. Doxygen generation enabled")
    add_custom_target(doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} .codedocs
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message(STATUS "Doxygen not found. Doxygen generation disabled")
endif ()


if (${LATEX_PDFLATEX_FOUND})
    message(STATUS "Latex PDF found. PDF generation enabled")
    add_custom_target(latexpdf
            DEPENDS doxygen
            COMMAND ${CMAKE_COMMAND} -E chdir ${ARTIFACT_DIR}/Doxygen/latex ${DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${ARTIFACT_DIR}/Doxygen
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            USES_TERMINAL
            )
else ()
    message(STATUS "Latex PDF not found. PDF generation disabled")
endif ()

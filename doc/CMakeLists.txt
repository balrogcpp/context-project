project(xio-doc)

find_package(Doxygen QUIET COMPONENTS dot)
find_package(LATEX QUIET COMPONENTS PDFLATEX)

# add a target to generate API documentation with Doxygen
if (NOT DOXYGEN_FOUND)
    return()
endif ()

# set input and output files
set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)


# request to configure the file
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc-doxygen
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_ARTIFACT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen..."
        VERBATIM
        )

if (UNIX)
    set(DOXYGEN_MAKE_COMMAND ${MAKE_COMMAND})
elseif (WIN32)
    set(DOXYGEN_MAKE_COMMAND make.bat)
endif ()
if (${LATEX_PDFLATEX_FOUND})
    add_custom_target(doc-pdf
            DEPENDS doc-doxygen
            COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_ARTIFACT_DIR}/Doxygen/latex ${DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${CONTEXT_ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${CONTEXT_ARTIFACT_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            VERBATIM
            )
endif ()

set(CMAKE_FOLDER Doc)
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

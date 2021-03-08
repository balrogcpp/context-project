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

project(xio-doc)

find_package(Doxygen QUIET COMPONENTS dot)
find_package(LATEX QUIET COMPONENTS PDFLATEX)

# add a target to generate API documentation with Doxygen
if (NOT DOXYGEN_FOUND)
    return()
endif ()

# set input and output files
set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CONTEXT_WORK_DIR}/Doxyfile)

# request to configure the file
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doxygen
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_ARTIFACT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen..."
        VERBATIM
        )

if (UNIX)
    set(DOXYGEN_MAKE_COMMAND ${CONTEXT_MAKE})
elseif (WIN32)
    set(DOXYGEN_MAKE_COMMAND make.bat)
endif ()

if (${LATEX_PDFLATEX_FOUND})
    add_custom_target(pdf
            DEPENDS doxygen
            COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_ARTIFACT_DIR}/Doxygen/latex ${DOXYGEN_MAKE_COMMAND}
            COMMAND ${CMAKE_COMMAND} -E copy ${CONTEXT_ARTIFACT_DIR}/Doxygen/latex/refman.pdf ${CONTEXT_ARTIFACT_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen..."
            VERBATIM
            )
endif ()

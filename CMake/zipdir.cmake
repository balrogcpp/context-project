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

macro(subdirlist result curdir)
   file(GLOB children RELATIVE ${curdir}/*)
   set(dirlist "")
   foreach (child ${children})
       if (IS_DIRECTORY ${curdir}/${child})
           list(APPEND dirlist ${child})
       endif ()
   endforeach ()
   set(${result} ${dirlist})
endmacro()

macro(zipdirlist curdir destination)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_TEMP_DIR}/${destination})
   file(GLOB children RELATIVE ${curdir} ${curdir}/*)
   set(dirlist "")
   foreach (child ${children})
       set(filelist "")
       if (IS_DIRECTORY ${curdir}/${child})
           file(GLOB children2 RELATIVE ${curdir}/${child} ${curdir}/${child}/*)
           foreach (child2 ${children2})
               if (NOT IS_DIRECTORY ${curdir}/${child}/${child2})
                   list(APPEND filelist ${child2})
               endif ()
           endforeach ()
           list(APPEND dirlist ${child})
           execute_process(COMMAND ${CMAKE_COMMAND} -E chdir "${curdir}/${child}/" ${CMAKE_COMMAND} -E tar "cf" "${CONTEXT_TEMP_DIR}/${destination}/${child}.zip" --format=zip ${filelist})
       endif ()
   endforeach ()
endmacro()

macro(copydirlist curdir destination)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CONTEXT_TEMP_DIR}/${destination})
   execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${curdir} ${CONTEXT_TEMP_DIR}/${destination})
endmacro()

set(CONTEXT_PROGRAMS_DIR ${CMAKE_SOURCE_DIR}/programs)
set(CONTEXT_SCENES_DIR ${CMAKE_SOURCE_DIR}/scenes)
set(CONTEXT_TMP_DIR ${CMAKE_SOURCE_DIR}/tmp)
zipdirlist(${CONTEXT_PROGRAMS_DIR} ${CONTEXT_TMP_DIR}/programs)
zipdirlist(${CONTEXT_SCENES_DIR} ${CONTEXT_TMP_DIR}/scenes)

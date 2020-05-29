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
#

cmake_minimum_required(VERSION 3.15.1)

include(cotire)

project(ContextProjectDemo)

set(HEADER_FILES_context-demo
    include/MenuAppState.h
    include/DemoDotAppState.h
    )

set(SOURCE_FILES_context-demo
    src/main.cpp
    src/MenuAppState.cpp
    src/DemoDotAppState.cpp
    )
include_directories(context-demo ${CMAKE_CURRENT_SOURCE_DIR}/include ${CONTEXT_DEPENDENCY_INCLUDE_DIRS})

if (NOT ANDROID)
    add_executable(context-demo ${SOURCE_FILES_context-demo} ${HEADER_FILES_context-demo})
else ()
    add_library(context-demo SHARED ${SOURCE_FILES_context-demo} ${HEADER_FILES_context-demo})
endif ()
set(CONTEXT_LIBRARIES context-static)

target_link_directories(context-demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})

target_link_libraries(context-demo PUBLIC ${CONTEXT_LIBRARIES} ${CONTEXT_STATIC_DEPENDENCY_LIBS})

set(CMAKE_CXX_FLAGS ${CONTEXT_CXX_FLAGS})
set(CMAKE_C_FLAGS ${CONTEXT_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${CONTEXT_EXE_LINKER_FLAGS})
set(CMAKE_STATIC_LINKER_FLAGS ${CONTEXT_STATIC_LINKER_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS ${CONTEXT_SHARED_LINKER_FLAGS})

install(TARGETS context-demo
        CONFIGURATIONS Release
        RUNTIME DESTINATION .
        )

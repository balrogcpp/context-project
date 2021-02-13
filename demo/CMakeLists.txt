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

cmake_minimum_required(VERSION 3.16.3)

project(xio-demo)

set(HEADER_FILES_context-demo
    include/DemoDotAppState.h
    include/MenuAppState.h
    include/pcheader.h
    )

set(SOURCE_FILES_context-demo
    src/main.cpp
    src/DemoDotAppState.cpp
    src/MenuAppState.cpp
    )

if (ANDROID)
    list(APPEND SOURCE_FILES_context ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    list(APPEND SOURCE_FILES_context ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()

if (NOT ANDROID)
    add_executable(demo ${SOURCE_FILES_context-demo} ${HEADER_FILES_context-demo})
else ()
    add_library(demo SHARED ${SOURCE_FILES_context-demo} ${HEADER_FILES_context-demo})
endif ()
set(CONTEXT_LIBRARIES xio-static)

include_directories(demo ${CMAKE_CURRENT_SOURCE_DIR}/include ${CONTEXT_DEPENDENCY_INCLUDE_DIRS} ${ANDROID_NDK}/sources/android/native_app_glue)
target_link_directories(demo PUBLIC ${CONTEXT_EXTERNAL_LIB_DIR} ${CONTEXT_EXTERNAL_LIB_DIR}/OGRE ${CONTEXT_LIB_DIR})
target_link_libraries(demo PUBLIC ${CONTEXT_LIBRARIES} ${CONTEXT_STATIC_DEPENDENCY_LIBS})
target_precompile_headers(demo PUBLIC include/pcheader.h)
install(TARGETS demo
        CONFIGURATIONS Release
        RUNTIME DESTINATION .
        )

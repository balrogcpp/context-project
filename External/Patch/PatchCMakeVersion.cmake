# include guard
if (_patch_cmake_included)
    return()
endif (_patch_cmake_included)
set(_patch_cmake_included true)

if (EXISTS CMakeLists.txt)
    file(READ CMakeLists.txt TEXT)
    string(REGEX MATCH "VERSION ([0-9/.]*)" _ "${TEXT}")

    if (CMAKE_MATCH_1 AND CMAKE_MATCH_1 VERSION_LESS 3.15)
        message("${CMAKE_MATCH_1}")
        string(REPLACE "VERSION ${CMAKE_MATCH_1}" "VERSION 3.15" TEXT "${TEXT}")
        file(WRITE CMakeLists.txt "${TEXT}")
    endif ()
endif ()

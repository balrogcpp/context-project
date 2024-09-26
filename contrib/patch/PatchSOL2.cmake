# include guard
if (_patch_sol2_included)
    return()
endif (_patch_sol2_included)
set(_patch_sol2_included true)

if (EXISTS CMakeLists.txt)
    file(READ CMakeLists.txt TEXT)
    string(FIND "${TEXT}" "EXACT REQUIRED" matchres)
    if(NOT ${matchres} EQUAL -1)
        string(REPLACE "EXACT REQUIRED" "EXACT" TEXT "${TEXT}")
        file(WRITE CMakeLists.txt "${TEXT}")
    endif ()
endif ()

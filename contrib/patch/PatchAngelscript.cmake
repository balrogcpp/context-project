# include guard
if (_patch_angelscript_included)
    return()
endif (_patch_angelscript_included)
set(_patch_angelscript_included true)

if(NOT EXISTS CMakeLists.txt)
    file(COPY sdk/angelscript/projects/cmake/CMakeLists.txt DESTINATION  .)
endif ()

if(NOT EXISTS cmake)
    file(COPY sdk/angelscript/projects/cmake/cmake DESTINATION  .)
endif ()

if (EXISTS CMakeLists.txt)
    file(READ CMakeLists.txt TEXT)
    string(REPLACE "../../include" "sdk/angelscript/include" TEXT "${TEXT}")
    string(REPLACE "../../source" "sdk/angelscript/source" TEXT "${TEXT}")
    file(WRITE CMakeLists.txt "${TEXT}")
endif ()

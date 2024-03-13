# include guard
if (_cpackpostbuild_included)
    return()
endif (_cpackpostbuild_included)
set(_cpackpostbuild_included true)


# clears timestamp to 1 January 1999 00:00:00
macro(ClearTimeStamp file)
    if (CMAKE_HOST_UNIX)
        execute_process(COMMAND touch -t 199901010000 ${file})
    elseif (CMAKE_HOST_WIN32)
        execute_process(COMMAND powershell "(Get-Item ${file}).CreationTime = (\"1 January 1999 00:00:00\")")
        execute_process(COMMAND powershell "(Get-Item ${file}).LastWriteTime = (\"1 January 1999 00:00:00\")")
        execute_process(COMMAND powershell "(Get-Item ${file}).LastAccessTime = (\"1 January 1999 00:00:00\")")
    endif ()
endmacro()


if (EXISTS ${CPACK_OUTPUT_FILE_PATH})
    ClearTimeStamp(${CPACK_OUTPUT_FILE_PATH})
endif ()

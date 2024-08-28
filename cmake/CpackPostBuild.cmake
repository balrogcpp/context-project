# include guard
if (_cpackpostbuild_included)
    return()
endif (_cpackpostbuild_included)
set(_cpackpostbuild_included true)


# clears timestamp
macro(ClearTimeStamp file)
    if (CMAKE_HOST_UNIX)
        execute_process(COMMAND touch -a -m -t 198001011201 ${file})
    elseif (CMAKE_HOST_WIN32)
        execute_process(COMMAND powershell "(Get-Item ${file}).CreationTime = (\"1 January 1980 12:01:00\")")
        execute_process(COMMAND powershell "(Get-Item ${file}).LastWriteTime = (\"1 January 1980 12:01:00\")")
        execute_process(COMMAND powershell "(Get-Item ${file}).LastAccessTime = (\"1 January 1980 12:01:00\")")
    endif ()
endmacro()

if (EXISTS ${CPACK_OUTPUT_FILE_PATH})
    ClearTimeStamp(${CPACK_OUTPUT_FILE_PATH})
    find_program(STRIP_EXE NAMES strip-nondeterminism)
    if (STRIP_EXE)
        execute_process(COMMAND ${STRIP_EXE} ${destination})
    endif ()
endif ()

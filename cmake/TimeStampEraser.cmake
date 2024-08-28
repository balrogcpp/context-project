# include guard
if (_timestamp_included)
    return()
endif (_timestamp_included)
set(_timestamp_included true)


# clears timestamp
macro(ClearTimeStampDirectory destination)
    if (CMAKE_HOST_UNIX)
        execute_process(COMMAND find ${destination} -exec touch -a -m -t 198001011201 {} +)
    elseif (CMAKE_HOST_WIN32)
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | ForEach-Object{$_.CreationTime = (\"1 January 1980 12:01:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | ForEach-Object{$_.LastWriteTime = (\"1 January 1980 12:01:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | ForEach-Object{$_.LastAccessTime = (\"1 January 1980 12:01:00\")}")
    endif ()
endmacro()

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

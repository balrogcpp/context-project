# include guard
if (_timestamp_included)
    return()
endif (_timestamp_included)
set(_timestamp_included true)


# clears timestamp to 1 January 1999 00:00:00
macro(ClearTimeStampDirectory destination)
    if (CMAKE_HOST_UNIX)
        execute_process(COMMAND find ${destination} -type f -exec touch -t 199901010000 {} +)
    elseif (CMAKE_HOST_WIN32)
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.CreationTime = (\"1 January 1999 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastWriteTime = (\"1 January 1999 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastAccessTime = (\"1 January 1999 00:00:00\")}")
    endif ()
endmacro()


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


if (IS_DIRECTORY ${TARGET})
    ClearTimeStampDirectory(${TARGET})
elseif (EXISTS ${TARGET})
    ClearTimeStamp(${TARGET})
elseif(TARGET)
    message(STATUS "Skip missing ${TARGET}")
endif ()

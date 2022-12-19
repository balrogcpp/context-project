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


if (IS_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY})
    ClearTimeStampDirectory(${CPACK_TEMPORARY_INSTALL_DIRECTORY})
endif ()

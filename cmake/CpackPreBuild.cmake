# include guard
if (_cpackprebuild_included)
    return()
endif (_cpackprebuild_included)
set(_cpackprebuild_included true)


# clears timestamp
macro(ClearTimeStampDirectory destination)
    if (CMAKE_HOST_UNIX)
        execute_process(COMMAND find ${destination} -type f -exec touch -t 201901010000 {} +)
    elseif (CMAKE_HOST_WIN32)
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.CreationTime = (\"1 January 2019 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastWriteTime = (\"1 January 2019 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastAccessTime = (\"1 January 2019 00:00:00\")}")
    endif ()
endmacro()

if (IS_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY})
    ClearTimeStampDirectory(${CPACK_TEMPORARY_INSTALL_DIRECTORY})
endif ()

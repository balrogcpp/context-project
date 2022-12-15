# include guard
if (_assets_included)
    return()
endif (_assets_included)
set(_assets_included true)


# Zip files from directory into flat zip
macro(FlatCopy curdir destination)
    make_directory(${destination})
    file(GLOB directories RELATIVE ${curdir} ${curdir}/*)

    foreach (directory ${directories})
        if (${directory} STREQUAL ".git") #omit .git directory
            continue()
        endif ()
        set(filelist "")
        if (IS_DIRECTORY ${curdir}/${directory})
            file(GLOB_RECURSE files LIST_DIRECTORIES false ${curdir}/${directory} ${curdir}/${directory}/*)
            file(COPY ${files} DESTINATION ${destination})
        endif ()
    endforeach ()
endmacro()


# Zip files from directory into flat zip
macro(ZipDirectory destination extention)
    file(GLOB filelist RELATIVE ${destination} ${destination}/*)
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.${extention} --format=zip ${filelist})
    file(REMOVE_RECURSE ${destination})
endmacro()


# Zip files from directory into flat zip
macro(FlatZipDirectory curdir destination extention)
    make_directory(${destination})
    file(GLOB directories RELATIVE ${curdir} ${curdir}/*)

    foreach (directory ${directories})
        if (${directory} STREQUAL ".git") #omit .git directory
            continue()
        endif ()
        set(filelist "")
        if (IS_DIRECTORY ${curdir}/${directory})
            file(GLOB_RECURSE files LIST_DIRECTORIES false ${curdir}/${directory} ${curdir}/${directory}/*)
            file(COPY ${files} DESTINATION ${destination})
        else ()
            file(COPY ${curdir}/${directory} DESTINATION ${destination})
        endif ()
    endforeach ()

    file(GLOB filelist RELATIVE ${destination} ${destination}/*)

    if (UNIX)
        execute_process(COMMAND find ${destination} -type f -exec touch -t 199901010000 {} +)
    elseif (WIN32)
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.CreationTime = (\"1 January 1999 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastWriteTime = (\"1 January 1999 00:00:00\")}")
        execute_process(COMMAND powershell "Get-ChildItem -force ${destination} * | Where-Object {! $_.PSIsContainer} | ForEach-Object{$_.LastAccessTime = (\"1 January 1999 00:00:00\")}")
    endif ()

    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.${extention} --format=zip ${filelist})

    if (UNIX)
        execute_process(COMMAND touch -t 199901010000 ${destination}.${extention})
    else ()
        execute_process(COMMAND powershell "(Get-Item ${destination}.${extention}).CreationTime = (\"1 January 1999 00:00:00\")")
        execute_process(COMMAND powershell "(Get-Item ${destination}.${extention}).LastWriteTime = (\"1 January 1999 00:00:00\")")
        execute_process(COMMAND powershell "(Get-Item ${destination}.${extention}).LastAccessTime = (\"1 January 1999 00:00:00\")")
    endif ()

    file(REMOVE_RECURSE ${destination})
endmacro()


# do the job
FlatZipDirectory(${INPUT} ${OUTPUT} ${FORMAT})

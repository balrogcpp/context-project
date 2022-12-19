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

    find_program(ZIP_EXE NAMES zip PATHS ENV PATH NO_DEFAULT_PATH)

    file(GLOB filelist RELATIVE ${destination} ${destination}/*)
    ClearTimeStampDirectory(${destination})
    if (ZIP_EXE)
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} zip -rq -D -X -9 -A ${destination}.${extention} ${filelist})
    else ()
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.${extention} --format=zip ${filelist})
    endif ()
    ClearTimeStamp(${destination}.${extention})
    file(REMOVE_RECURSE ${destination})
endmacro()


# do the job
FlatZipDirectory(${INPUT} ${OUTPUT} ${FORMAT})

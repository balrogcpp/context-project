macro(FlatZipSubdirectories curdir destination)
    make_directory(${destination})
    file(GLOB directories RELATIVE ${curdir} ${curdir}/*)

    foreach (directory ${directories})
        if (${directory} STREQUAL ".git") #omit .git directory
            continue()
        endif ()
        set(filelist "")
        if (IS_DIRECTORY ${curdir}/${directory})
            file(GLOB_RECURSE files LIST_DIRECTORIES false ${curdir}/${directory} ${curdir}/${directory}/*)
            make_directory(${destination}/${directory})
            file(COPY ${files} DESTINATION ${destination}/${directory})
            file(GLOB filelist RELATIVE ${destination}/${directory} ${destination}/${directory}/*)
            execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}/${directory}.zip --format=zip ${filelist})
            file(REMOVE_RECURSE ${destination}/${directory})
        endif ()
    endforeach ()
endmacro()


# Zip files from directory into flat zip
macro(CopyFlatZipDirectory curdir destination)
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
macro(ZipDirectory destination)
    file(GLOB filelist RELATIVE ${destination} ${destination}/*)
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.zip --format=zip ${filelist})
    file(REMOVE_RECURSE ${destination})
endmacro()


# Zip files from directory into flat zip
macro(FlatZipDirectory curdir destination)
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

    file(GLOB filelist RELATIVE ${destination} ${destination}/*)
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.zip --format=zip ${filelist})
    file(REMOVE_RECURSE ${destination})
endmacro()

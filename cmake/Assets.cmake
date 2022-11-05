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
            execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}/${directory}.bin --format=zip ${filelist})
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
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.bin --format=zip ${filelist})
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
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.bin --format=zip ${filelist})
    file(REMOVE_RECURSE ${destination})
endmacro()


file(REMOVE_RECURSE ${ASSETS_DIR})
make_directory(${ASSETS_DIR})
CopyFlatZipDirectory(${ROOT_DIR}/source/Programs ${ASSETS_DIR}/programs)
if (ANDROID OR IOS OR EMSCRIPTEN OR GLSLES)
    file(GLOB files ${ROOT_DIR}/source/GLSLES/*)
else ()
    file(GLOB files ${ROOT_DIR}/source/GLSL/*)
endif ()
file(COPY ${files} DESTINATION ${ASSETS_DIR}/programs)
FlatZipDirectory(${ROOT_DIR}/source/Example/Assets ${ASSETS_DIR}/assets)
ZipDirectory(${ASSETS_DIR}/programs)

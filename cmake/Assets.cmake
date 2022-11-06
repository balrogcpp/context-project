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
    execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.${extention} --format=zip ${filelist})
    file(REMOVE_RECURSE ${destination})
endmacro()


# do the job
FlatCopy(${ROOT_DIR}/source/Programs ${ASSETS_DIR}/programs)
if (ANDROID OR IOS OR EMSCRIPTEN OR GLSLES)
    file(GLOB files ${ROOT_DIR}/source/GLSLES/*)
else ()
    file(GLOB files ${ROOT_DIR}/source/GLSL/*)
endif ()
file(COPY ${files} DESTINATION ${ASSETS_DIR}/programs)
ZipDirectory(${ASSETS_DIR}/programs bin)

FlatZipDirectory(${ROOT_DIR}/source/Programs ${ASSETS_DIR}/programs zip)
FlatZipDirectory(${ROOT_DIR}/source/GLSL ${ASSETS_DIR}/glsl zip)
FlatZipDirectory(${ROOT_DIR}/source/GLSLES ${ASSETS_DIR}/glsles zip)
FlatZipDirectory(${ROOT_DIR}/source/Example/Assets ${ASSETS_DIR}/assets bin)

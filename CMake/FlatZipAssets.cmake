macro(flat_zip curdir destination)
    file(MAKE_DIRECTORY ${destination})
    file(GLOB directories RELATIVE ${curdir} ${curdir}/*)
    foreach (directory ${directories})
        if (${directory} STREQUAL ".git") #omit .git directory
            continue()
        endif ()
        set(filelist "")
        if (IS_DIRECTORY ${curdir}/${directory})
            file(GLOB_RECURSE files LIST_DIRECTORIES false ${curdir}/${directory} ${curdir}/${directory}/*)
            file(MAKE_DIRECTORY ${destination}/${directory})
            file(COPY ${files} DESTINATION ${destination}/${directory})
            file(GLOB filelist RELATIVE ${destination}/${directory} ${destination}/${directory}/*)
            execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}/${directory}.zip --format=zip ${filelist})
            file(REMOVE_RECURSE ${destination}/${directory})
        endif ()
    endforeach ()
endmacro()


set(GLUE_TMP_DIR ${CMAKE_SOURCE_DIR}/Tmp)
file(REMOVE_RECURSE ${GLUE_TMP_DIR})
flat_zip(${CMAKE_SOURCE_DIR}/Programs ${GLUE_TMP_DIR}/Programs)
flat_zip(${CMAKE_SOURCE_DIR}/Assets ${GLUE_TMP_DIR}/Assets)
# include guard
if (_package_included)
    return()
endif (_package_included)
set(_package_included true)


# Zip files from directory into flat zip
macro(FlatZipDirectory curdir destination)
    get_filename_component(DESTINATION_PARENT_DIR ${destination} DIRECTORY ABSOLUTE)
    if (DESTINATION_PARENT_DIR)
        file(MAKE_DIRECTORY ${DESTINATION_PARENT_DIR})
    endif ()
    get_filename_component(PARENT_DIR ${curdir} DIRECTORY ABSOLUTE)
    file(GLOB_RECURSE filelist LIST_DIRECTORIES false RELATIVE ${PARENT_DIR} ${curdir}/*)
    find_program(ZIP_EXE NAMES zip)
    execute_process(COMMAND ${ZIP_EXE} --version OUTPUT_VARIABLE ZIP_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "Zip ([0-9/.]*)" _ "${ZIP_VERSION}")
    set(ZIP_VERSION ${CMAKE_MATCH_1})
    find_program(TAR_EXE NAMES bsdtar tar)
    execute_process(COMMAND ${TAR_EXE} --version OUTPUT_VARIABLE TAR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    find_package(Java COMPONENTS Development QUIET)
    find_program(STRIP_EXE NAMES strip-nondeterminism)

    # bsdtar generates same output as 'cmake -E tar --format=zip' on linux
    # on windows cmake tar is not stable, so using windows build-in tar instead
    # by default use zip command if available, as it avoids generation of extra field
    if (ZIP_EXE AND ZIP_VERSION VERSION_GREATER_EQUAL 3.0)
        message("Using zip ${ZIP_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${ZIP_EXE} -FSrq -D -X ${destination} ${filelist})
    elseif (TAR_EXE AND TAR_VERSION MATCHES zlib)
        string(REGEX MATCH "bsdtar ([0-9/.]*)" _ "${TAR_VERSION}")
        set(TAR_VERSION ${CMAKE_MATCH_1})
        message("Zip not found. Using bsdtar ${TAR_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${TAR_EXE} caf ${destination} ${filelist})
    elseif (Java_Development_FOUND)
        message("Zip or bsdtar not found. Using jar ${Java_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${Java_JAR_EXECUTABLE} -cfM ${destination} ${filelist})
    else ()
        message("Zip or bsdtar not found. Using cmake ${CMAKE_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${CMAKE_COMMAND} -E tar cf ${destination} --format=zip ${filelist})
    endif ()
    if (STRIP_EXE)
        execute_process(COMMAND ${STRIP_EXE} ${destination})
    endif ()
endmacro()

# do the job
FlatZipDirectory(${INPUT} ${OUTPUT})

# include guard
if (_assets_included)
    return()
endif (_assets_included)
set(_assets_included true)


# Zip files from directory into flat zip
macro(FlatCopy curdir destination)
    file(MAKE_DIRECTORY ${destination})
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
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${ZIP_EXE} -rq -D -X -9 -A ${destination}.${extention} ${filelist})
    else ()
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${destination}/${directory} ${CMAKE_COMMAND} -E tar cf ${destination}.${extention} --format=zip ${filelist})
    endif ()
    ClearTimeStamp(${destination}.${extention})
    file(REMOVE_RECURSE ${destination})
endmacro()


# do the job
FlatZipDirectory(${INPUT} ${OUTPUT} ${FORMAT})

if (NOT GENERATE_HEADER)
    return()
endif ()

file(WRITE Zip2Cpp.cpp
        "#define _CRT_SECURE_NO_WARNINGS\n"
        "\n"
        "#include <fstream>\n"
        "#include <sys/stat.h>\n"
        "#include <string>\n"
        "\n"
        "\n"
        "void help(void)\n"
        "{\n"
        "// Print help message\n"
        "printf(\"OgreZip2Cpp: Convert a zip file into a cpp file.\\n\");\n"
        "printf(\"Usage: OgreFile2Cpp [zip file name without extension (without .zip)]\\n\");\n"
        "printf(\"The working folder needs to be the same as the one of the zip file.\\n\");\n"
        "}\n"
        "\n"
        "// the main function\n"
        "int main(int numargs, char** args)\n"
        "{\n"
        "// check that we have only one parameter - the zip file name without extension\n"
        "if (numargs != 2)\n"
        "{\n"
        "printf(\"Error: Wrong number of parameters (the program uses one parameter)\\n\");\n"
        "help();\n"
        "return -1;\n"
        "}\n"
        "std::string fileNameWithoutExt = args[1];\n"
        "std::string zipFileName = fileNameWithoutExt + \".zip\";\n"
        "std::string cppFileName = zipFileName + \".h\";\n"
        "std::string structName = fileNameWithoutExt;\n"
        "\n"
        "\n"
        "// get the zip file size\n"
        "std::streamsize fileSize = 0;\n"
        "struct stat results;\n"
        "if (stat(zipFileName.c_str(), &results) == 0)\n"
        "// The size of the file in bytes is in\n"
        "// results.st_size\n"
        "{\n"
        "fileSize = results.st_size;\n"
        "}\n"
        "\n"
        "// check that the file is valid\n"
        "if (fileSize == 0)\n"
        "{\n"
        "printf(\"Error: Zip file was not found or its size is zero (make sure that you didn't add .zip at the end of the parameter)\\n\");\n"
        "help();\n"
        "return -2;\n"
        "}\n"
        "\n"
        "// alloc memory for the in(zip) file - loading it all to memory is much fater then\n"
        "// one char at a time\n"
        "unsigned char * inFileData = new unsigned char[fileSize];\n"
        "\n"
        "// open, read the data to memory and close the in file\n"
        "std::ifstream roStream;\n"
        "roStream.open(zipFileName.c_str(),  std::ifstream::in | std::ifstream::binary);\n"
        "roStream.read((char *)inFileData, fileSize);\n"
        "roStream.close();\n"
        "\n"
        "// alloc the out file, the out file is a text file, meaning it is much bigger then the input file\n"
        "// allocating a buffer ten time bigger then the size of the in file - just to be on the safe side.\n"
        "// waste of memory, yes, but the most easy way to be safe without writing lots of code\n"
        "char * outFileData = new char[fileSize * 10];\n"
        "char * outFileDataPos = outFileData;\n"
        "\n"
        "\n"
        "// start writing out the out cpp file\n"
        "// ----------------------------------\n"
        "\n"
        "// add the include\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"#pragma once\\n\\n\");\n"
        "\n"
        "// declare and init the content of the zip file to a static buffer\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"constexpr unsigned char %s_zip[] = \\n\", fileNameWithoutExt.c_str());\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"{\\n    \");\n"
        "int posCurOutInLine = 0;\n"
        "for( std::streamsize i = 0 ;  i < fileSize ; i++ )\n"
        "{\n"
        "// get the current char\n"
        "unsigned char outChar = inFileData[i];\n"
        "\n"
        "// if you want to encrypt the data - encrypt the char here\n"
        "// ....\n"
        "\n"
        "// play with the formatting so the data will look nice\n"
        "// add spaces before small number\n"
        "if(outChar < 10)\n"
        "outFileDataPos+= sprintf(outFileDataPos, \" \");\n"
        "if(outChar < 100)\n"
        "outFileDataPos+= sprintf(outFileDataPos, \" \");\n"
        "\n"
        "// output the char to the out cpp file\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"%d\", outChar);\n"
        "posCurOutInLine+=3;\n"
        "\n"
        "// add a comman after every char but the last\n"
        "if(i + 1 != fileSize) {\n"
        "outFileDataPos+= sprintf(outFileDataPos, \", \");\n"
        "posCurOutInLine+=2;\n"
        "}\n"
        "\n"
        "// cut the line every 100 chars\n"
        "if (posCurOutInLine > 100) {\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"\\n    \");\n"
        "posCurOutInLine = 0;\n"
        "}\n"
        "}\n"
        "\n"
        "// close the static buffer that holds the file data\n"
        "outFileDataPos+= sprintf(outFileDataPos, \"\\n};\\n\");\n"
        "\n"
        "// write out the cpp file\n"
        "std::ofstream writeStream;\n"
        "writeStream.open(cppFileName.c_str(),  std::ifstream::out | std::ofstream::binary);\n"
        "writeStream.write(outFileData, ((std::streamsize)(outFileDataPos - outFileData)));\n"
        "writeStream.close();\n"
        "\n"
        "// done, let go home and drink some beers\n"
        "return 0;\n"
        "}\n"
        "\n"
)

if (NOT CPP_COMPILER)
    find_program(CPP_COMPILER NAMES c++ clang++ g++ cl bcc32 bcc xlc PATHS ENV PATH NO_DEFAULT_PATH)
endif ()
if (NOT CPP_COMPILER)
    message(FATAL_ERROR "CXX compiler not found manually")
else ()
    message(STATUS "CXX compiler found manually: ${CPP_COMPILER}")
endif ()

execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} ${CPP_COMPILER} ${CMAKE_CURRENT_BINARY_DIR}/Zip2Cpp.cpp -o zip2cpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${ASSETS_DIR} ./zip2cpp programs)
execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf ${ASSETS_DIR}/programs.zip)

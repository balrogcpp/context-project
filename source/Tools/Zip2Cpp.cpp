// _CRT_SECURE_NO_WARNINGS is defined to disable this compiler warning:
// "
//  warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.
// To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// "
#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sys/stat.h>
#include <string>


void help(void)
{
  // Print help message
  printf("OgreZip2Cpp: Convert a zip file into a cpp file.\n");
  printf("Usage: OgreFile2Cpp [zip file name without extension (without .zip)]\n");
  printf("The working folder needs to be the same as the one of the zip file.\n");
}

// the main function
int main(int numargs, char** args)
{
  // check that we have only one parameter - the zip file name without extension
  if (numargs != 2)
  {
    printf("Error: Wrong number of parameters (the program uses one parameter)\n");
    help();
    return -1;
  }
  std::string fileNameWithoutExt = args[1];
  std::string zipFileName = fileNameWithoutExt + ".zip";
  std::string cppFileName = zipFileName + ".h";
  std::string structName = fileNameWithoutExt;


  // get the zip file size
  std::streamsize fileSize = 0;
  struct stat results;
  if (stat(zipFileName.c_str(), &results) == 0)
    // The size of the file in bytes is in
    // results.st_size
  {
    fileSize = results.st_size;
  }

  // check that the file is valid
  if (fileSize == 0)
  {
    printf("Error: Zip file was not found or its size is zero (make sure that you didn't add .zip at the end of the parameter)\n");
    help();
    return -2;
  }

  // alloc memory for the in(zip) file - loading it all to memory is much fater then
  // one char at a time
  unsigned char * inFileData = new unsigned char[fileSize];

  // open, read the data to memory and close the in file
  std::ifstream roStream;
  roStream.open(zipFileName.c_str(),  std::ifstream::in | std::ifstream::binary);
  roStream.read((char *)inFileData, fileSize);
  roStream.close();

  // alloc the out file, the out file is a text file, meaning it is much bigger then the input file
  // allocating a buffer ten time bigger then the size of the in file - just to be on the safe side.
  // waste of memory, yes, but the most easy way to be safe without writing lots of code
  char * outFileData = new char[fileSize * 10];
  char * outFileDataPos = outFileData;


  // start writing out the out cpp file
  // ----------------------------------

  // add the include
  outFileDataPos+= sprintf(outFileDataPos, "#pragma once\n\n");

  // declare and init the content of the zip file to a static buffer
  outFileDataPos+= sprintf(outFileDataPos, "constexpr unsigned char %s_zip[] = \n", fileNameWithoutExt.c_str());
  outFileDataPos+= sprintf(outFileDataPos, "{\n    ");
  int posCurOutInLine = 0;
  for( std::streamsize i = 0 ;  i < fileSize ; i++ )
  {
    // get the current char
    unsigned char outChar = inFileData[i];

    // if you want to encrypt the data - encrypt the char here
    // ....

    // play with the formatting so the data will look nice
    // add spaces before small number
    if(outChar < 10)
      outFileDataPos+= sprintf(outFileDataPos, " ");
    if(outChar < 100)
      outFileDataPos+= sprintf(outFileDataPos, " ");

    // output the char to the out cpp file
    outFileDataPos+= sprintf(outFileDataPos, "%d", outChar);
    posCurOutInLine+=3;

    // add a comman after every char but the last
    if(i + 1 != fileSize) {
      outFileDataPos+= sprintf(outFileDataPos, ", ");
      posCurOutInLine+=2;
    }

    // cut the line every 100 chars
    if (posCurOutInLine > 100) {
      outFileDataPos+= sprintf(outFileDataPos, "\n    ");
      posCurOutInLine = 0;
    }
  }

  // close the static buffer that holds the file data
  outFileDataPos+= sprintf(outFileDataPos, "\n};\n");

  // write out the cpp file
  std::ofstream writeStream;
  writeStream.open(cppFileName.c_str(),  std::ifstream::out | std::ofstream::binary);
  writeStream.write(outFileData, ((std::streamsize)(outFileDataPos - outFileData)));
  writeStream.close();

  // done, let go home and drink some beers
  return 0;
}


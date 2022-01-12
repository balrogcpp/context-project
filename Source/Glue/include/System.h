// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once

#include "Platform.h"

#ifdef DESKTOP

#include <string>

namespace Glue {

std::string GetCurrentDirectoryB(const std::string &args);
std::string GetUserDirectory();
std::string PathAppend(const std::string &Path, const std::string &Append);
std::string TrunkPath(std::string &Path);
bool DirectoryExists(const std::string &Path);
void CreateDirectory(const std::string &Path);

}

#endif

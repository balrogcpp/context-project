// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "System.h"

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <windows.h>
#elif defined(UNIX)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace Glue {

std::string GetCurrentDirectoryB(const std::string &args) {
#if defined(UNIX)
  if (args.empty()) return "";

  std::string aux(args);

  // Get the last position of '/'
  int pos = aux.rfind('/');

  // Get the path and the name
  std::string path = aux.substr(0, pos + 1);
  std::string name = aux.substr(pos + 1);

  return path.append("/");
#elif defined(WINDOWS)
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  std::string::size_type pos = std::string(buffer).find_last_of("\\/");

  std::string path = std::string(buffer).substr(0, pos);

  return path.append("\\");
#endif

  return "";
}

std::string GetUserDirectory() {
#if defined(UNIX)
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  if (homedir) return homedir;
#elif defined(WINDOWS)
  char buffer[MAX_PATH];
  if (SHGetSpecialFolderPathA(NULL, buffer, CSIDL_LOCAL_APPDATA, false)) return buffer;
#endif

  return "";
}

}  // namespace Glue

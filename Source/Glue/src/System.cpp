// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "System.h"
#include "Filesystem.h"

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <windows.h>
#elif defined(UNIX)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace std;

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

std::string TrunkPath(std::string &Path) {
#if defined(UNIX)
  return Path.append("/");
#elif defined(WINDOWS)
  return Path.append("\\");
#endif
}

std::string PathAppend(const std::string &Path, const std::string &Append) {
  std::string Result = Path;

#if defined(UNIX)
  Result.append("/");
  Result.append(Append);
  return Result;
#elif defined(WINDOWS)
  Result.append("\\");
  Result.append(Append);
  return Result;
#endif

  return "";
}

bool DirectoryExists(const std::string &Path) { return fs::exists(Path); }

void CreateDirectory(const std::string &Path) {
  if (!fs::exists(Path)) fs::create_directory(Path);
}

}  // namespace Glue

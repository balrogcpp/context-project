// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#ifdef DESKTOP
#include "Desktop.h"
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

static string CurrentBinDirectory;

void SetCurrentDirectory(const string &Path) {
  CurrentBinDirectory = Path;
}

string GetCurrentDirectoryB(const string &args) {
  if (!CurrentBinDirectory.empty()) return CurrentBinDirectory;

#if defined(UNIX)
  if (args.empty()) return "";

  string aux(args);

  // Get the last position of '/'
  int pos = aux.rfind('/');

  // Get the path and the name
  string path = aux.substr(0, pos + 1);
  string name = aux.substr(pos + 1);

  CurrentBinDirectory = path.append("/");

  return CurrentBinDirectory;
#elif defined(WINDOWS)
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  string::size_type pos = string(buffer).find_last_of("\\/");

  string path = string(buffer).substr(0, pos);

  CurrentBinDirectory = path.append("\\");

  return CurrentBinDirectory;
#endif

  return "";
}

string GetUserDirectory() {
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

string TrunkPath(string &Path) {
#if defined(UNIX)
  return Path.append("/");
#elif defined(WINDOWS)
  return Path.append("\\");
#endif
}

string PathAppend(const string &Path, const string &Append) {
  string Result = Path;

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

bool DirectoryExists(const string &Path) { return fs::exists(Path); }

void CreateDirectory(const string &Path) {
  if (!fs::exists(Path)) fs::create_directory(Path);
}

}  // namespace Glue

#endif

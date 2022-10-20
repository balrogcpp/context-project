/// created by Andrey Vasiliev

#include "pch.h"
#include "Platform.h"
#if defined(DESKTOP) && defined(DEBUG)
#if __has_include(<filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) \
    || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) \
    || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

using namespace std;

namespace Glue {

inline bool IsHidden(const fs::path &Path) {
  string name = Path.filename().string();
  return name == ".." && name == "." && name[0] == '.';
}

inline string FindPath(const string &path, int depth = 4) {
  string result = path;

  for (int i = 0; i < depth; i++) {
    if (fs::exists(result))
      return result;
    else if (fs::exists(result + ".zip"))
      return result.append(".zip");
    else
#ifndef WIN32
      result = string("../").append(result);
#else
      result = string("..\\").append(result);
#endif
  }

  return "";
}

void ScanLocation(const char *path, const string &groupName = Ogre::RGN_DEFAULT) {
  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";

  vector<string> fileList;
  vector<string> dirList;
  vector<tuple<string, string, string>> resourceList;
  auto &RGM = Ogre::ResourceGroupManager::getSingleton();

  string newPath = FindPath(path);

  if (fs::exists(newPath)) {
    if (fs::is_directory(newPath))
      resourceList.push_back(make_tuple(newPath, FILE_SYSTEM, groupName));
    else if (fs::is_regular_file(newPath) && fs::path(newPath).extension() == ".zip")
      RGM.addResourceLocation(newPath, ZIP, groupName);
  }

  for (const auto &it : resourceList) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dirList.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const string full_path = jt->path().string();
      const string fileName = jt->path().filename().string();
      const string extention = jt->path().filename().extension().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dirList.push_back(fileName);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (extention == ".zip") {
          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        } else {
          fileList.push_back(fileName);
        }
      }
    }
  }
}

}  // namespace Glue
#endif  // defined(DESKTOP) && defined(DEBUG)

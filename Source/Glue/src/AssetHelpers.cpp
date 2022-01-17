// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"

#ifdef DESKTOP

#include "AssetHelpers.h"
#include "Exception.h"
#include "Filesystem.h"
#include "System.h"

using namespace std;
using namespace Ogre;

namespace Glue {

static inline string FindPath(string Path, int Depth = 2) {
#ifdef DEBUG
  Depth = 6;
#endif

  string result = Path;

  fs::current_path(GetCurrentDirectoryB(""));

  for (int i = 0; i < Depth; i++) {
    if (fs::exists(result))
      return result;
    else if (fs::exists(result + ".zip"))
      return result.append(".zip");
    else
      result = string("../").append(result);
  }

  return string();
}

static inline bool IsAllowed(char c) { return isalpha(c) || isdigit(c) || c == '.' || c == ',' || c == ';' || c == '_' || c == '-' || c == '/'; }

static inline bool StringSanityCheck(const string &str) {
  if (str.empty() || str[0] == '#') {
    return true;
  }

  return any_of(str.begin(), str.end(), IsAllowed);
}

static inline void LeftTrim(string &s) {
  auto it = find_if(s.begin(), s.end(), [](char c) { return !isspace(c); });
  s.erase(s.begin(), it);
}

static inline void RightTrim(string &s) {
  auto it = find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); });
  s.erase(it.base(), s.end());
}

static inline void TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}

static void LoadResources() {
  auto &rgm = ResourceGroupManager::getSingleton();
  rgm.initialiseResourceGroup(RGN_INTERNAL);
  rgm.initialiseAllResourceGroups();
}

static inline bool IsHidden(const fs::path &p) {
  string name = p.filename().string();
  return name.compare("..") && name.compare(".") && name[0] == '.';
}

template <typename T, typename C>
bool IsIn(const T &t, const C &list) {
  return find(begin(list), end(list), t) != end(list);
}

const static std::vector<std::string> EXTENTION_LIST = {
    ".glsl", ".glslt", ".hlsl", ".hlslt", ".gles", ".cg",   ".vert", ".frag",  ".material", ".compositor", ".particle", ".fx",  ".program", ".dds",
    ".bmp",  ".png",   ".tga",  ".jpg",   ".jpeg", ".mesh", ".xml",  ".scene", ".json",     ".wav",        ".ogg",      ".mp3", ".flac"};

static inline bool IsInExtList(string ext) { return IsIn(ext, EXTENTION_LIST); }

void AddLocation(const std::string &Path, const std::string &GroupName) {
  const string FILE_SYSTEM = "FileSystem";
  const string ZIP = "Zip";

  std::vector<string> file_list;
  std::vector<string> dir_list;
  std::vector<tuple<string, string, string>> resource_list;
  auto &RGM = ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    if (fs::is_directory(path))
      resource_list.push_back(make_tuple(path, FILE_SYSTEM, GroupName));
    else if (fs::is_regular_file(path) && fs::path(path).extension() == ".zip")
      RGM.addResourceLocation(path, ZIP, GroupName);
  }

  for (const auto &it : resource_list) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const string full_path = jt->path().string();
      const string file_name = jt->path().filename().string();
      const string extention = jt->path().filename().extension().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dir_list.push_back(file_name);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (extention == ".zip") {
          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        } else if (IsInExtList(extention)) {
          file_list.push_back(file_name);
        }
      }
    }
  }
}

void AddResourceFile(const std::string &Path, const std::string &GroupName, const std::string &ResourceFile) {
  const string FILE_SYSTEM = "FileSystem";
  const string ZIP = "Zip";

  std::vector<string> file_list;
  std::vector<string> dir_list;
  std::vector<tuple<string, string, string>> resource_list;
  auto &RGM = ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    if (fs::is_directory(path))
      resource_list.push_back(make_tuple(path, FILE_SYSTEM, GroupName));
    else if (fs::is_regular_file(path) && fs::path(path).extension() == ".zip")
      RGM.addResourceLocation(path, ZIP, GroupName);
  }

  if (!ResourceFile.empty()) {
    fstream list_file;
    list_file.open(ResourceFile);

    string line;
    string type;
    string group;

    if (list_file.is_open()) {
      while (getline(list_file, line)) {
        TrimString(line);

        if (!StringSanityCheck(line)) {
          throw Glue::Exception(string("Sanity check of ") + ResourceFile + " is failed. Aborting.");
        }

        if (line[0] == '#') {
          continue;
        }

        stringstream ss(line);
        getline(ss, path, ',');
        getline(ss, type, ',');
        getline(ss, group, ';');
        resource_list.push_back(make_tuple(path, type, group));
      }
    }
  }

  for (const auto &it : resource_list) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const string full_path = jt->path().string();
      const string file_name = jt->path().filename().string();
      const string extention = jt->path().filename().extension().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dir_list.push_back(file_name);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (extention == ".zip") {
          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        } else if (IsInExtList(extention)) {
          file_list.push_back(file_name);
        }
      }
    }
  }
}

}  // namespace Glue

#endif  // DESKTOP

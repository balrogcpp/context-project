// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Exception.h"
#include "AssetHelpers.h"
#include "Filesystem.h"

using namespace std;
using namespace Ogre;

namespace Glue {

#ifdef DESKTOP

static inline string FindPath(string Path, int Depth = 2) {
#ifdef DEBUG
  Depth = 6;
#endif

  string result = Path;

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

void AddLocation(const std::string &Path, const std::string &GroupName, const std::string &ResourceFile) {
  const string FILE_SYSTEM = "FileSystem";
  const string ZIP = "Zip";

  std::vector<string> file_list;
  std::vector<string> dir_list;
  std::vector<tuple<string, string, string>> resource_list;
  auto &RGM = ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    if (fs::is_directory(path))
      resource_list.push_back({path, FILE_SYSTEM, GroupName});
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
        resource_list.push_back({path, type, group});
      }
    }
  }

  const std::vector<string> extensions_list = {".glsl",       ".glslt",    ".hlsl", ".hlslt",   ".gles", ".cg",  ".vert", ".frag", ".material",
                                               ".compositor", ".particle", ".fx",   ".program", ".dds",  ".bmp", ".png",  ".tga",  ".jpg",
                                               ".jpeg",       ".mesh",     ".xml",  ".scene",   ".json", ".wav", ".ogg",  ".mp3",  ".flac"};

  for (const auto &it : resource_list) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const auto full_path = jt->path().string();
      const auto file_name = jt->path().filename().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dir_list.push_back(file_name);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (fs::path(full_path).extension() == ".zip") {
          if (find(extensions_list.begin(), extensions_list.end(), fs::path(file_name).extension()) != extensions_list.end()) {
            file_list.push_back(file_name);
          }

          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        }
      }
    }
  }
}

#endif  // !MOBILE

}
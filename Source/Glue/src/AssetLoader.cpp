// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "AssetLoader.h"
#include "Android.h"
#include "Exception.h"
#include "Log.h"
#if HAS_FILESYSTEM
#include <filesystem>
namespace fs = filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

using namespace std;

namespace Glue {

string AssetLoader::FindPath(const string &Path, int Depth) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS

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

#else
  return Path;
#endif
}

static bool CheckSymbol(char c) {
  return isalpha(c) || isdigit(c) || c == '.' || c == ',' || c == ';' || c == '_' || c == '-' || c == '/';
}

static bool StringSanityCheck(const string &str) {
  if (str.empty() || str[0] == '#') {
    return true;
  }

  return any_of(str.begin(), str.end(), CheckSymbol);
}

static void LeftTrim(string &s) {
  auto it = find_if(s.begin(), s.end(), [](char c) { return !isspace(c); });
  s.erase(s.begin(), it);
}

static void RightTrim(string &s) {
  auto it = find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); });
  s.erase(it.base(), s.end());
}

static void TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}

void AssetLoader::LoadResources() {
  auto &rgm = Ogre::ResourceGroupManager::getSingleton();
  rgm.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  rgm.initialiseAllResourceGroups();
}

void AssetLoader::AddLocation(const string &Path, const string &GroupName, bool Recursive) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
  const string file_system = "FileSystem";
  const string zip = "Zip";

  string result = FindPath(Path);
  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  if (!result.empty()) {
    if (fs::is_directory(result)) {
      rgm.addResourceLocation(result, file_system, GroupName, Recursive);
    } else if (fs::path(result).extension() == ".zip") {
      rgm.addResourceLocation(result, zip, GroupName, Recursive);
    }
  }
#else
  Assert(false, "AssetLoader::AddLocation called in another platform");
#endif
}

void AssetLoader::AddLocationAndroid(AAssetManager *AssetManager, const string &ResourceFile, const string &GroupName,
                                     bool Verbose) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//  const string file_system = "APKFileSystem";
//
//  AAssetDir *dir = AAssetManager_openDir(AssetManager, path_.c_str());
//  const char *fileName = nullptr;
//
//  while ((fileName = AAssetDir_getNextFileName(dir)) != nullptr) {
//    LogMessage(string("Found directory: ") + fileName);
//  }
#else
  Assert(false, "AssetLoader::AddLocationAndroid called in another platform");
#endif
}

void AssetLoader::AddLocationRecursive(const string &Path, const string &GroupName, const string &ResourceFile,
                                       bool Verbose) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
  const string file_system = "FileSystem";
  const string zip = "Zip";

  vector<string> file_list;
  vector<string> dir_list;
  vector<tuple<string, string, string>> resource_list;
  auto &orm = Ogre::ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    resource_list.push_back({path, file_system, GroupName});
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
          throw Exception(string("Sanity check of ") + ResourceFile + " is failed. Aborting.");
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

  const static vector<string> extensions_list = {
      ".glsl",       ".glslt",    ".hlsl", ".hlslt",   ".gles", ".cg",  ".vert", ".frag", ".material",
      ".compositor", ".particle", ".fx",   ".program", ".dds",  ".bmp", ".png",  ".tga",  ".jpg",
      ".jpeg",       ".mesh",     ".xml",  ".scene",   ".json", ".wav", ".ogg",  ".mp3",  ".flac"};

  for (const auto &it : resource_list) {
    orm.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    auto jt = fs::recursive_directory_iterator(get<0>(it));

    for (; jt != fs::recursive_directory_iterator(); ++jt) {
      const auto full_path = jt->path().string();
      const auto file_name = jt->path().filename().string();

      if (jt->is_directory()) {
        if (Verbose) {
          LogMessage(string("Found directory: ") + full_path);
        }

        dir_list.push_back(file_name);

        orm.addResourceLocation(full_path, file_system, get<2>(it));

      } else if (jt->is_regular_file()) {
        if (Verbose) {
          LogMessage(string("Found file: ") + full_path);
        }
        if (fs::path(full_path).extension() == ".zip") {
          if (find(extensions_list.begin(), extensions_list.end(), fs::path(file_name).extension()) !=
              extensions_list.end()) {
            file_list.push_back(file_name);
          }

          orm.addResourceLocation(full_path, zip, get<2>(it));
        }
      }
    }
  }
#else
  Assert(false, "AssetLoader::AddLocationRecursive called in another platform");
#endif
}

}  // namespace Glue

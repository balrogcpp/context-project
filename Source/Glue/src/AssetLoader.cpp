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

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS

string AssetLoader::FindPath(const string &path, int depth) {
#ifdef DEBUG
  depth = 6;
#endif

  string result = path;

  for (int i = 0; i < depth; i++) {
    if (fs::exists(result))
      return result;
    else if (fs::exists(result + ".zip"))
      return result.append(".zip");
    else
      result = string("../").append(result);
  }

  return string();
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

void AssetLoader::TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}
#endif

void AssetLoader::LoadResources() {
  auto &rgm = Ogre::ResourceGroupManager::getSingleton();
  rgm.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  rgm.initialiseAllResourceGroups();
}

void AssetLoader::AddLocation(const string &path, const string &group, bool recursive) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
  const string file_system = "FileSystem";
  const string zip = "Zip";

  string result = FindPath(path);
  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  if (!result.empty()) {
    if (fs::is_directory(result)) {
      rgm.addResourceLocation(result, file_system, group, recursive);
    } else if (fs::path(result).extension() == ".zip") {
      rgm.addResourceLocation(result, zip, group, recursive);
    }
  }
#else
  Assert(false, "AssetLoader::AddLocation called in another platform");
#endif
}

void AssetLoader::AddLocationAndroid(AAssetManager *asset_mgr, const string &resource_file, const string &group,
                                      bool verbose) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//  const string file_system = "APKFileSystem";
//
//  AAssetDir *dir = AAssetManager_openDir(asset_mgr, path_.c_str());
//  const char *fileName = nullptr;
//
//  while ((fileName = AAssetDir_getNextFileName(dir)) != nullptr) {
//    LogMessage(string("Found directory: ") + fileName);
//  }
#else
  Assert(false, "AssetLoader::AddLocationAndroid called in another platform");
#endif
}

void AssetLoader::AddLocationRecursive(const string &path_, const string &group, const string &resource_file,
                                        bool verbose) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
  const string file_system = "FileSystem";
  const string zip = "Zip";

  vector<string> file_list;
  vector<string> dir_list;
  vector<tuple<string, string, string>> resource_list;
  auto &orm = Ogre::ResourceGroupManager::getSingleton();

  string path = FindPath(path_);

  if (fs::exists(path)) {
    resource_list.push_back({path, file_system, group});
  }

  if (!resource_file.empty()) {
    fstream list_file;
    list_file.open(resource_file);

    string line;
    string type;
    string group;

    if (list_file.is_open()) {
      while (getline(list_file, line)) {
        TrimString(line);

        if (!StringSanityCheck(line)) {
          throw Exception(string("Sanity check of ") + resource_file + " is failed. Aborting.");
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
        if (verbose) {
          LogMessage(string("Found directory: ") + full_path);
        }

        dir_list.push_back(file_name);

        orm.addResourceLocation(full_path, file_system, get<2>(it));

      } else if (jt->is_regular_file()) {
        if (verbose) {
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

}  // namespace glue

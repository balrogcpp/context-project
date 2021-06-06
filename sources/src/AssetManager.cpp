// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "AssetManager.h"

#include <iostream>

#include "Android.h"
#include "Exception.h"
#include "Log.h"
#include "pcheader.h"
#if HAS_FILESYSTEM
#include <filesystem>
namespace fs = filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

using namespace std;

namespace xio::AssetManager {

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//----------------------------------------------------------------------------------------------------------------------
static string FindPath(const string &path_, int depth = 3) {
  string path = path_;

  for (int i = 0; i < depth; i++) {
    if (fs::exists(path))
      return path;
    else if (fs::exists(path + ".zip"))
      return path.append(".zip");
    else
      path = string("../").append(path);
  }

  return string();
}

//----------------------------------------------------------------------------------------------------------------------
static bool CheckSymbol(char c) {
  return (isalpha(c) || isdigit(c) || c == '.' || c == ',' || c == ';' || c == '_' || c == '-' || c == '/');
}

//----------------------------------------------------------------------------------------------------------------------
static bool StringSanityCheck(const string &str) {
  if (str.empty() || str[0] == '#') {
    return true;
  }

  return any_of(str.begin(), str.end(), CheckSymbol);
}

//----------------------------------------------------------------------------------------------------------------------
static void LeftTrim(string &s) {
  auto it = find_if(s.begin(), s.end(), [](char c) { return !isspace(c); });
  s.erase(s.begin(), it);
}

//----------------------------------------------------------------------------------------------------------------------
static void RightTrim(string &s) {
  auto it = find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); });
  s.erase(it.base(), s.end());
}

//----------------------------------------------------------------------------------------------------------------------
static void TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void LoadResources() {
  auto &rgm = Ogre::ResourceGroupManager::getSingleton();
  rgm.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  rgm.initialiseAllResourceGroups();
}

//----------------------------------------------------------------------------------------------------------------------
void AddLocation(const string &path_, const string &group_, bool recursive) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  const string file_system = "FileSystem";
  const string zip = "Zip";

  string path = FindPath(path_);
  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  if (!path.empty()) {
    if (fs::is_directory(path)) {
      rgm.addResourceLocation(path, file_system, group_, recursive);
    } else if (fs::path(path).extension() == ".zip") {
      rgm.addResourceLocation(path, zip, group_, recursive);
    }
  }

#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AddLocationAndroid(AAssetManager *asset_mgr, const string &resource_file, const string &group_, bool verbose) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
//  const string file_system = "APKFileSystem";
//
//
//  AAssetDir* dir = AAssetManager_openDir(asset_mgr, path_.c_str());
//  const char* fileName = NULL;
//
//  while((fileName = AAssetDir_getNextFileName(dir)) != NULL) {
//	LogMessage(string("Found directory: ")
//+ fileName);
//  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AddLocationRecursive(const string &path_, const string &group_, const string &resource_file, bool verbose) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  const string file_system = "FileSystem";
  const string zip = "Zip";

  vector<string> file_list;
  vector<string> dir_list;
  vector<tuple<string, string, string>> resource_list;
  auto &orm = Ogre::ResourceGroupManager::getSingleton();

  string path = FindPath(path_);

  if (fs::exists(path)) {
    resource_list.push_back({path, file_system, group_});
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
          throw Exception(string("Sanity check of file ") + resource_file + " is failed. Aborting.");
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
#endif
}

}  // namespace xio::AssetManager

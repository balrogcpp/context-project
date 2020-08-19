/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <Ogre.h>

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <tuple>

namespace storage {

class StorageException : public std::exception {
 public:
  StorageException() = default;

  explicit StorageException(std::string description)
      : description(std::move(description)) {};

  ~StorageException() noexcept override = default;

 public:
  std::string getDescription() const noexcept {
    return description;
  }

  const char *what() const noexcept override {
    return description.c_str();
  }

 protected:
  std::string description = std::string("Description not specified");
  size_t code = 0;
};

//----------------------------------------------------------------------------------------------------------------------
inline bool StringSanityCheck(const std::string &str) {
  if (str[0] == '#') {
    return true;
  }

  for (const auto &it : str) {
    if (std::isalpha(it) || std::isdigit(it) || it == '.' || it == ',' || it == ';' || it == '_' || it == '-' || it == '/') {
    } else {
      return false;
    }
  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
inline void LeftTrim(std::string &s) {
  auto it = std::find_if(s.begin(), s.end(), [](char c) {
    return !std::isspace<char>(c, std::locale::classic());
  });
  s.erase(s.begin(), it);
}
//----------------------------------------------------------------------------------------------------------------------
inline void RightTrim(std::string &s) {
  auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
  s.erase(it.base(), s.end());
}
//----------------------------------------------------------------------------------------------------------------------
inline void TrimString(std::string &s) {
  RightTrim(s);
  LeftTrim(s);
}
//----------------------------------------------------------------------------------------------------------------------
inline void PrintPathList(const std::vector<std::tuple<std::string, std::string, std::string>> &path_list) {
  std::cout << "Path list:\n";

  for (const auto &it : path_list) {
    std::cout << "Path: " << std::get<0>(it) << "; Type: " << std::get<1>(it) << "; Group: " << std::get<2>(it)
              << ";\n";
  }

  std::cout << '\n';
}
//----------------------------------------------------------------------------------------------------------------------
inline void PrintStringList(const std::vector<std::string> &string_list) {
  std::cout << "Path list:\n";

  for (const auto &it : string_list) {
    std::cout << "String : " << it << ";\n";
  }

  std::cout << '\n';
}
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::tuple<std::string, std::string, std::string>>
InitGeneralResources(const std::vector<std::string> &path_list, const std::string &resource_file = "", bool verbose = false) {
  namespace fs = std::filesystem;
  const std::string default_group_name = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

  std::vector<std::string> file_list;
  std::vector<std::string> dir_list;
  std::vector<std::tuple<std::string, std::string, std::string>> resource_list;
  auto &ogre_resource_manager = Ogre::ResourceGroupManager::getSingleton();

//#ifndef DEBUG
//  media_location_directory_ = "./";
//#else
//  media_location_directory_ = "../../../";
//#endif

//  resource_list.push_back({path + "programs", "FileSystem", default_group_name});
//  resource_list.push_back({path + "scenes", "FileSystem", default_group_name});

//#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
//  if (rtss_cache_enable_) {
//    if (!std::filesystem::exists(rtss_cache_dir_)) {
//      std::filesystem::create_directory(rtss_cache_dir_);
//      resource_list.push_back({rtss_cache_dir_, "FileSystem", default_group_name});
//    }
//  }
//#endif
  for (const auto &it : path_list)
    resource_list.push_back({it, "FileSystem", default_group_name});

  if (!resource_file.empty()) {
    std::fstream list_file;
    list_file.open(resource_file);

    std::string line;
    std::string path;
    std::string type;
    std::string group;

    if (list_file.is_open()) {
      while (getline(list_file, line)) {
        TrimString(line);

        if (!StringSanityCheck(line)) {
          throw StorageException(std::string("Sanity check of file ") + resource_file + " is failed. Aborting.");
        }

        if (line[0] == '#') {
          continue;
        }

        std::stringstream ss(line);
        getline(ss, path, ',');
        getline(ss, type, ',');
        getline(ss, group, ';');
        resource_list.push_back({path, type, group});
      }

      list_file.close();
    }
  }

  if (verbose)
    PrintPathList(resource_list);

  const std::vector<std::string> extensions_list =
      {".glsl", ".glslt", ".hlsl", ".hlslt", ".gles", ".cg", ".vert", ".frag", ".material", ".compositor", ".particle",
       ".fx", ".program", ".dds", ".bmp", ".png", ".tga", ".jpg",
       ".jpeg", ".mesh", ".xml", ".scene", ".json", ".wav", ".ogg", ".mp3", ".flac"};

  for (const auto &it : resource_list) {
    ogre_resource_manager.addResourceLocation(std::get<0>(it), std::get<1>(it), std::get<2>(it));
    if (std::find(std::begin(path_list), std::end(path_list), std::get<0>(it)) == std::end(path_list)) {
      dir_list.push_back(std::get<0>(it));
    } else {
      //throw StorageException("Path " + std::get<0>(it) + " already registered. Aborting.");
    }

#ifndef __ANDROID__
    for (auto jt = fs::recursive_directory_iterator(std::get<0>(it)); jt != fs::recursive_directory_iterator(); ++jt) {
      const auto file_path = jt->path().string();
      const auto file_name = jt->path().filename().string();

      if (jt->is_directory()) {
        if (verbose) {
          std::cout << "Parsing directory:  " << file_path << '\n';
        }
        if (std::find(std::begin(path_list), std::end(path_list), file_name) == std::end(path_list)) {
          dir_list.push_back(file_name);
        } else {
          //throw StorageException("Path " + file_name + " already registered. Aborting.");
        }

        ogre_resource_manager.addResourceLocation(file_path, "FileSystem", std::get<2>(it));

      } else if (jt->is_regular_file()) {
        if (verbose) {
          std::cout << "Parsing file:  " << file_path << '\n';
        }
        if (fs::path(file_path).extension() == ".zip") {
          if (std::find(std::begin(file_list), std::end(file_list), file_name) == std::end(file_list)) {
            if (std::find(std::begin(extensions_list), std::end(extensions_list), fs::path(file_name).extension())
                != std::end(extensions_list)) {
              file_list.push_back(file_name);
            }
          } else {
            //throw StorageException("File " + file_name + " already exists. Aborting.");
          }

          ogre_resource_manager.addResourceLocation(file_path, "Zip", std::get<2>(it));
        }
      }
    }
#else
    if (std::get<1>(it) == "Filesystem" || std::get<1>(it) == "Zip")
      ogre_resource_manager.addResourceLocation(std::get<0>(it), std::get<1>(it), std::get<2>(it));
#endif
  }

  // load resources
  Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

  return resource_list;
}
} //namespace storage

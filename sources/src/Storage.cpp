//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "Storage.h"

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
bool Storage::StringSanityCheck(const std::string &str) {
  if (str[0]=='#') {
	return true;
  }

  for (const auto &it : str) {
	if (std::isalpha(it) || std::isdigit(it) || it=='.' || it==',' || it==';' || it=='_' || it=='-'
		|| it=='/') {
	} else {
	  return false;
	}
  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void Storage::LeftTrim(std::string &s) {
  auto it = std::find_if(s.begin(), s.end(), [](char c) {
	return !std::isspace<char>(c, std::locale::classic());
  });
  s.erase(s.begin(), it);
}
//----------------------------------------------------------------------------------------------------------------------
void Storage::RightTrim(std::string &s) {
  auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
  s.erase(it.base(), s.end());
}
//----------------------------------------------------------------------------------------------------------------------
void Storage::TrimString(std::string &s) {
  RightTrim(s);
  LeftTrim(s);
}
//----------------------------------------------------------------------------------------------------------------------
void Storage::PrintPathList(const std::vector<std::tuple<std::string, std::string, std::string>> &path_list) {
  std::cout << "Path list:\n";

  for (const auto &it : path_list) {
	std::cout << "Path: " << std::get<0>(it) << "; Type: " << std::get<1>(it) << "; Group: " << std::get<2>(it)
			  << ";\n";
  }

  std::cout << '\n';
}
//----------------------------------------------------------------------------------------------------------------------
void Storage::PrintStringList(const std::vector<std::string> &string_list) {
  std::cout << "Path list:\n";

  for (const auto &it : string_list) {
	std::cout << "String : " << it << ";\n";
  }

  std::cout << '\n';
}

void Storage::LoadResources() {
  Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::tuple<std::string, std::string, std::string>>
Storage::InitGeneralResources(const std::vector<std::string> &path_list,
							  const std::string &resource_file,
							  bool verbose) {
  namespace fs = std::filesystem;
  const std::string default_group_name = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

  std::vector<std::string> file_list;
  std::vector<std::string> dir_list;
  std::vector<std::tuple<std::string, std::string, std::string>> resource_list;
  auto &ogre_resource_manager = Ogre::ResourceGroupManager::getSingleton();
#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
  for (const auto &it : path_list) {
	if (fs::exists(it))
	  resource_list.push_back({it, "FileSystem", default_group_name});
  }
#endif
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

		if (line[0]=='#') {
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
	  {".glsl", ".glslt", ".hlsl", ".hlslt", ".gles", ".cg", ".vert", ".frag", ".material", ".compositor",
	   ".particle",
	   ".fx", ".program", ".dds", ".bmp", ".png", ".tga", ".jpg",
	   ".jpeg", ".mesh", ".xml", ".scene", ".json", ".wav", ".ogg", ".mp3", ".flac"};

  for (const auto &it : resource_list) {
	ogre_resource_manager.addResourceLocation(std::get<0>(it), std::get<1>(it), std::get<2>(it));
	if (std::find(std::begin(path_list), std::end(path_list), std::get<0>(it))==std::end(path_list)) {
	  dir_list.push_back(std::get<0>(it));
	} else {
	  //throw StorageException("Path " + std::get<0>(it) + " already registered. Aborting.");
	}

#ifndef __ANDROID__
	for (auto jt = fs::recursive_directory_iterator(std::get<0>(it)); jt!=fs::recursive_directory_iterator();
		 ++jt) {
	  const auto file_path = jt->path().string();
	  const auto file_name = jt->path().filename().string();

	  if (jt->is_directory()) {
		if (verbose) {
		  std::cout << "Parsing directory:  " << file_path << '\n';
		}
		if (std::find(std::begin(path_list), std::end(path_list), file_name)==std::end(path_list)) {
		  dir_list.push_back(file_name);
		} else {
		  //throw StorageException("Path " + file_name + " already registered. Aborting.");
		}

		ogre_resource_manager.addResourceLocation(file_path, "FileSystem", std::get<2>(it));

	  } else if (jt->is_regular_file()) {
		if (verbose) {
		  std::cout << "Parsing file:  " << file_path << '\n';
		}
		if (fs::path(file_path).extension()==".zip") {
		  if (std::find(std::begin(file_list), std::end(file_list), file_name)==std::end(file_list)) {
			if (std::find(std::begin(extensions_list), std::end(extensions_list), fs::path(file_name).extension())
				!=std::end(extensions_list)) {
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

  return resource_list;
}

} //namespace

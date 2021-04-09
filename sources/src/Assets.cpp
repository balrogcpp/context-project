//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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
#include "Assets.h"
#include "Exception.h"
#include "Android.h"
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

namespace xio::Assets {

static std::string FindPath(const std::string &path_, int depth = 3) {
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
  return (isalpha(c) || isdigit(c) || c=='.' || c==',' || c==';' || c=='_' || c=='-' || c=='/');
}

//----------------------------------------------------------------------------------------------------------------------
static bool StringSanityCheck(const string &str) {
  if (str.empty() || str[0]=='#') {
	return true;
  }

  return any_of(str.begin(), str.end(), CheckSymbol);
}

//----------------------------------------------------------------------------------------------------------------------
static void LeftTrim(string &s) {
  auto it = find_if(s.begin(), s.end(), [](char c) { return !isspace < char > (c, locale::classic()); });
  s.erase(s.begin(), it);
}

//----------------------------------------------------------------------------------------------------------------------
static void RightTrim(string &s) {
  auto it = find_if(s.rbegin(), s.rend(), [](char c) { return !isspace < char > (c, locale::classic()); });
  s.erase(it.base(), s.end());
}

//----------------------------------------------------------------------------------------------------------------------
static void TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}

//----------------------------------------------------------------------------------------------------------------------
void PrintPathList(const vector <tuple<string, string, string>> &path_list) {
  cout << "Path list:\n";

  for (const auto &it : path_list) {
	cout << "Path: " << get<0>(it) << "; Type: " << get<1>(it) << "; Group: " << get<2>(it) << ";\n";
  }

  cout << '\n';
}

//----------------------------------------------------------------------------------------------------------------------
void PrintStringList(const vector <string> &string_list) {
  cout << "Path list:\n";

  for (const auto &it : string_list) {
	cout << "String : " << it << ";\n";
  }

  cout << '\n';
}

//----------------------------------------------------------------------------------------------------------------------
void LoadResources() {
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::RGN_INTERNAL);
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//----------------------------------------------------------------------------------------------------------------------
void AddLocation(const string &path_, const string &group_, bool recursive) {
#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
  const std::string file_system = "FileSystem";
  const std::string zip = "Zip";

  string path = FindPath(path_);
  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  if (!path.empty()) {
	if (fs::is_directory(path)) {

	  rgm.addResourceLocation(path, file_system, group_, recursive);

	} else if (fs::path(path).extension()==".zip") {
	  rgm.addResourceLocation(path, zip, group_, recursive);
	}
  }

#endif

}

//----------------------------------------------------------------------------------------------------------------------
void AddLocationAndroid(AAssetManager* asset_mgr, const string &resource_file, const string &group_, bool verbose) {
#if OGRE_PLATFORM==OGRE_PLATFORM_ANDROID
//  const string file_system = "APKFileSystem";
//
//
//  AAssetDir* dir = AAssetManager_openDir(asset_mgr, path_.c_str());
//  const char* fileName = NULL;
//
//  while((fileName = AAssetDir_getNextFileName(dir)) != NULL) {
//	Ogre::LogManager::getSingleton().logMessage(string("Found directory: ") + fileName);
//  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AddLocationRecursive(const string &path_,
								  const string &group_,
								  const string &resource_file,
								  bool verbose) {
#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
  const string file_system = "FileSystem";
  const string zip = "Zip";

  vector <string> file_list;
  vector <string> dir_list;
  vector <tuple<string, string, string>> resource_list;
  auto &ogre_resource_manager = Ogre::ResourceGroupManager::getSingleton();

  string path = FindPath(path_);

  if (fs::exists(path))
	resource_list.push_back({path, file_system, group_});

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

		if (line[0]=='#') {
		  continue;
		}

		stringstream ss(line);
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

  const vector <string> extensions_list =
	  {".glsl", ".glslt", ".hlsl", ".hlslt", ".gles", ".cg", ".vert", ".frag", ".material", ".compositor",
	   ".particle",
	   ".fx", ".program", ".dds", ".bmp", ".png", ".tga", ".jpg",
	   ".jpeg", ".mesh", ".xml", ".scene", ".json", ".wav", ".ogg", ".mp3", ".flac"};

  for (const auto &it : resource_list) {
	ogre_resource_manager.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
	dir_list.push_back(get<0>(it));

	for (auto jt = fs::recursive_directory_iterator(get<0>(it)); jt!=fs::recursive_directory_iterator(); ++jt) {
	  const auto file_path = jt->path().string();
	  const auto file_name = jt->path().filename().string();

	  if (jt->is_directory()) {
		if (verbose) {
		  Ogre::LogManager::getSingleton().logMessage(string("Found directory: ") + file_path);
		}

		dir_list.push_back(file_name);

		ogre_resource_manager.addResourceLocation(file_path, file_system, get<2>(it));

	  } else if (jt->is_regular_file()) {
		if (verbose) {
		  Ogre::LogManager::getSingleton().logMessage(string("Found file: ") + file_path);
		}
		if (fs::path(file_path).extension()==".zip") {
		  if (find(begin(extensions_list), end(extensions_list), fs::path(file_name).extension())
			  !=end(extensions_list)) {
			file_list.push_back(file_name);
		  }

		  ogre_resource_manager.addResourceLocation(file_path, zip, get<2>(it));
		}
	  }
	}
  }
#endif
}

} //namespace

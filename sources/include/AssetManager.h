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

#pragma once

#include <Ogre.h>
#include <string>
#include <vector>
#include <tuple>

struct AAssetManager;

namespace xio {

namespace AssetManager {

void LoadResources();


void AddLocation(const std::string &path_,
				 const std::string &group_ = Ogre::RGN_DEFAULT,
				 bool recursive = false);


void AddLocationRecursive(const std::string &path_,
						  const std::string &group_ = Ogre::RGN_DEFAULT,
						  const std::string &resource_file = "",
						  bool verbose = false);


void AddLocationAndroid(AAssetManager *asset_mgr,
						const std::string &resource_file,
						const std::string &group_,
						bool verbose = false);
};

} //namespace

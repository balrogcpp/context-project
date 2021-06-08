// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once

#include <Ogre.h>

#include <string>
#include <tuple>
#include <vector>

struct AAssetManager;

namespace xio {

namespace AssetManager {

void LoadResources();

void AddLocation(const std::string &path_, const std::string &group_ = Ogre::RGN_DEFAULT, bool recursive = false);

void AddLocationRecursive(const std::string &path_, const std::string &group_ = Ogre::RGN_DEFAULT,
                          const std::string &resource_file = "", bool verbose = false);

void AddLocationAndroid(AAssetManager *asset_mgr, const std::string &resource_file, const std::string &group_,
                        bool verbose = false);
};  // namespace AssetManager

}  // namespace xio

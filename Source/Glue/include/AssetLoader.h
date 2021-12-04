// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreResourceGroupManager.h>
#include <string>

struct AAssetManager;

namespace Glue {

class AssetLoader final {
 public:
  ///
  static void LoadResources();

  ///
  static void AddLocation(const std::string &path, const std::string &group = Ogre::RGN_DEFAULT,
                          bool recursive = false);

  ///
  static void AddLocationRecursive(const std::string &path_, const std::string &group = Ogre::RGN_DEFAULT,
                                   const std::string &resource_file = "", bool verbose = false);

  ///
  static void AddLocationAndroid(AAssetManager *asset_mgr, const std::string &resource_file, const std::string &group,
                                 bool verbose = false);

  static std::string FindPath(const std::string &path, int depth = 2);

 private:
  static void TrimString(std::string &s);
};

}  // namespace Glue

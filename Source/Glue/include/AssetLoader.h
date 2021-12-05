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
  static void AddLocation(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT,
                          bool Recursive = false);

  ///
  static void AddLocationRecursive(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT,
                                   const std::string &ResourceFile = "", bool Verbose = false);

  ///
  static void AddLocationAndroid(AAssetManager *AssetManager, const std::string &ResourceFile,
                                 const std::string &GroupName = Ogre::RGN_DEFAULT, bool Verbose = false);

  static std::string FindPath(const std::string &Path, int Depth = 2);
};

}  // namespace Glue

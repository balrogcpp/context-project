/// created by Andrey Vasiliev

#ifdef NDEBUG
#include <OgreRoot.h>
#include <OgreZip.h>

#if __has_include("programs.zip.h")
#include "programs.zip.h"
#endif

//#if __has_include("assets.zip.h")
//#include "assets.zip.h"
//#endif

#endif

namespace Glue {
void InitEmbeddedResources() {
#ifdef NDEBUG
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();

#if __has_include("programs.zip.h")
  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("programs.zip", programs_zip, sizeof(programs_zip), nullptr);
  ogreResourceManager.addResourceLocation("programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
#endif

//#if __has_include("assets.zip.h")
//  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("assets.zip", assets_zip, sizeof(assets_zip), nullptr);
//  ogreResourceManager.addResourceLocation("assets.zip", "EmbeddedZip", Ogre::RGN_DEFAULT);
//#endif

#endif
}
}  // namespace Glue

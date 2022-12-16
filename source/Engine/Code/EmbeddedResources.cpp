/// created by Andrey Vasiliev

#include <OgreRoot.h>
#include <OgreZip.h>
#if defined(NDEBUG) || defined(__ANDROID__)
#if __has_include("programs.zip.h")
#include "programs.zip.h"
#endif
#endif

namespace Glue {
void InitEmbeddedResources() {
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
#if defined(NDEBUG) || defined(__ANDROID__)
#if __has_include("programs.zip.h")
  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("programs.zip", programs_zip, sizeof(programs_zip), nullptr);
  ogreResourceManager.addResourceLocation("programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
#endif
#endif
}
}  // namespace Glue

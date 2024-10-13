/// created by Andrey Vasiliev

#include <OgreResourceManager.h>
#include <OgreZip.h>

#if __has_include("Programs.h")
#include "Programs.h"
#endif

namespace gge {

bool InitEmbeddedResources() {
#if __has_include("Programs.h")
  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("Programs.zip", Programs_zip, sizeof(Programs_zip), nullptr);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
  return true;
#else
  return false;
#endif
}

}  // namespace gge

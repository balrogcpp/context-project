/// created by Andrey Vasiliev

#ifdef NDEBUG

#include <OgreRoot.h>
#include <OgreZip.h>

#if __has_include("programs.zip.h")
#include "programs.zip.h"
#endif

#endif

namespace gge {

void InitEmbeddedResources() {
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();

#if __has_include("programs.zip.h")
  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("programs.zip", programs_zip, sizeof(programs_zip), nullptr);
  ogreResourceManager.addResourceLocation("programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
#endif
}

}  // namespace gge

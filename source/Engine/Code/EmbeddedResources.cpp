/// created by Andrey Vasiliev

#include <OgreRoot.h>
#include <OgreZip.h>

#if __has_include("programs.h")
#include "programs.h"
#endif

namespace gge {

bool InitEmbeddedResources() {
#if __has_include("programs.h")
  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("programs.zip", programs_zip, sizeof(programs_zip), nullptr);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
  return true;
#else
  retune false;
#endif
}

}  // namespace gge

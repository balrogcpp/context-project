/// created by Andrey Vasiliev

#include "glsl.zip.h"
#include "glsles.zip.h"
#include "programs.zip.h"
#include <OgreRoot.h>
#include <OgreZip.h>

namespace {
inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
}  // namespace

namespace Glue {
void InitEmbeddedResources() {
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();

  Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("programs.zip", programs_zip, sizeof(programs_zip), nullptr);
  ogreResourceManager.addResourceLocation("programs.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);

  if (RenderSystemIsGLES2()) {
    Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("glsles.zip", glsles_zip, sizeof(glsles_zip), nullptr);
    ogreResourceManager.addResourceLocation("glsles.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
  } else {
    Ogre::EmbeddedZipArchiveFactory::addEmbbeddedFile("glsl.zip", glsl_zip, sizeof(glsl_zip), nullptr);
    ogreResourceManager.addResourceLocation("glsl.zip", "EmbeddedZip", Ogre::RGN_INTERNAL);
  }
}
}  // namespace Glue

/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <OgreRoot.h>
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#include <RenderSystems/GLES2/OgreGLUtil.h>

namespace gge {
void InitOgreRenderSystemGLES2() {
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());

  if (Ogre::getGLSupport()) {
    Ogre::LogManager::getSingleton().logMessage("Available video modes:");
    for (const auto &it : Ogre::getGLSupport()->getVideoModes()) {
      Ogre::LogManager::getSingleton().logMessage(it.getDescription());
    }
  }
}
}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GLES2

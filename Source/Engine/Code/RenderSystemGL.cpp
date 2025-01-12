/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GL
#include <OgreRoot.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLUtil.h>

namespace gge {
void InitOgreRenderSystemGL() {
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem());

  if (Ogre::getGLSupport()) {
    Ogre::LogManager::getSingleton().logMessage("Available video modes:");
    for (const auto &it : Ogre::getGLSupport()->getVideoModes()) {
      Ogre::LogManager::getSingleton().logMessage(it.getDescription());
    }
  }
}
}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GL

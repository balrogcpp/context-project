/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
#include <RenderSystems/GL3Plus/OgreGLUtil.h>

namespace gge {
void InitOgreRenderSystemGL3() {
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GL3PlusRenderSystem());

  if (Ogre::getGLSupport()) {
    Ogre::LogManager::getSingleton().logMessage("Available video modes:");
    for (const auto &it : Ogre::getGLSupport()->getVideoModes()) {
      Ogre::LogManager::getSingleton().logMessage(it.getDescription());
    }
  }
}
}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GL3PLUS

/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <OgreRoot.h>
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>

namespace Glue {
void InitOgreRenderSystemGLES2() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem()); }
}  // namespace Glue
#endif  // OGRE_BUILD_RENDERSYSTEM_GLES2

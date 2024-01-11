/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "pch.h"
#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <OgreRoot.h>
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>

namespace gge {

void InitOgreRenderSystemGLES2() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem()); }

}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GLES2

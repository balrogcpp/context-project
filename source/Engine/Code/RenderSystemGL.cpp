/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "pch.h"
#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GL
#include <OgreRoot.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>

namespace gge {

void InitOgreRenderSystemGL() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem()); }

}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GL

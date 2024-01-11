/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "pch.h"
#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <OgreRoot.h>
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>

namespace gge {

void InitOgreRenderSystemGL3() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GL3PlusRenderSystem()); }

}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_GL3PLUS

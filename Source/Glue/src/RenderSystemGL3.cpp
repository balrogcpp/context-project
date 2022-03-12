// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#include "Engine.h"

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS

#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL3() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GL3PlusRenderSystem()); }

}  // namespace Glue

#endif  // OGRE_BUILD_RENDERSYSTEM_GL3PLUS

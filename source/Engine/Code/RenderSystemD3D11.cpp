/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "pch.h"
#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_D3D11
#include <OgreRoot.h>
#include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>

namespace gge {

void InitOgreRenderSystemD3D11() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::D3D11RenderSystem()); }

}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_D3D11

/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_D3D9
#include <OgreRoot.h>
#include <RenderSystems/Direct3D9/OgreD3D9RenderSystem.h>

namespace gge {
void InitOgreRenderSystemD3D9() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::D3D9RenderSystem()); }
}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_D3D9

/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_VULKAN
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <RenderSystems/Vulkan/OgreVulkanRenderSystem.h>

namespace gge {
void InitOgreRenderSystemVulkan() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::VulkanRenderSystem()); }
}  // namespace gge
#endif  // OGRE_BUILD_RENDERSYSTEM_VULKAN

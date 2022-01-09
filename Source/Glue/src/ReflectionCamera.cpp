// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "ReflectionCamera.h"

using namespace std;

namespace Glue {

ReflectionCamera::ReflectionCamera(Ogre::Plane plane, unsigned int tex_size) {
  this->plane = plane;
  using namespace Ogre;
  auto size = tex_size;
  auto &tex_manager = Ogre::TextureManager::getSingleton();

  // create our reflection & refraction render textures, and setup their render
  // targets
  scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera = scene->getCamera("Default");

  rcamera = camera;
  rcamera->setAutoAspectRatio(false);

  reflection_texture = tex_manager.createManual("Reflection", RGN_DEFAULT, TEX_TYPE_2D, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  Ogre::RenderTarget *rtt1 = reflection_texture->getBuffer()->getRenderTarget();
  Ogre::Viewport *vp1 = rtt1->addViewport(rcamera);
  vp1->setOverlaysEnabled(false);
  vp1->setShadowsEnabled(false);
  // toggle reflection in camera
  rtt1->addListener(this);
  vp1->setVisibilityMask(SURFACE_MASK);

  refraction_texture = tex_manager.createManual("Refraction", RGN_DEFAULT, TEX_TYPE_2D, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  Ogre::RenderTarget *rtt2 = refraction_texture->getBuffer()->getRenderTarget();
  Ogre::Viewport *vp2 = rtt2->addViewport(camera);
  vp2->setOverlaysEnabled(false);
  vp2->setShadowsEnabled(false);
  rtt2->addListener(this);
  vp2->setVisibilityMask(SUBMERGED_MASK);

  rcamera->setAutoAspectRatio(true);
}

ReflectionCamera::~ReflectionCamera() { Clear(); }

void ReflectionCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  rcamera->enableReflection(plane);
  rcamera->setLodBias(0.001);
}

void ReflectionCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  rcamera->disableReflection();
  rcamera->setLodBias(1.0);
}

void ReflectionCamera::Clear() {
  if (reflection_texture) {
    Ogre::RenderTarget *rtt = reflection_texture->getBuffer()->getRenderTarget();
    rtt->removeAllListeners();
    rtt->removeAllViewports();
    Ogre::TextureManager::getSingleton().remove(reflection_texture);
  }

  if (refraction_texture) {
    Ogre::RenderTarget *rtt = refraction_texture->getBuffer()->getRenderTarget();
    rtt->removeAllListeners();
    rtt->removeAllViewports();
    Ogre::TextureManager::getSingleton().remove(refraction_texture);
  }
}

void ReflectionCamera::SetPlane(Ogre::Plane plane) { this->plane = plane; }

}  // namespace Glue

// This source file is part of context-project
// Created by Andrew Vasiliev

#include "ReflectionCamera.h"

#include "pcheader.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
ReflectionCamera::ReflectionCamera(Ogre::Plane plane, unsigned int tex_size) {
  SetPlane(plane);
  Init_(tex_size);
}

//----------------------------------------------------------------------------------------------------------------------
ReflectionCamera::~ReflectionCamera() { Clear_(); }

//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  rcamera_->enableReflection(plane_);
  rcamera_->setLodBias(0.001);
}

//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  rcamera_->disableReflection();
  rcamera_->setLodBias(1.0);
}

//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Clear_() {
  if (reflection_tex_) {
    Ogre::RenderTarget *rtt = reflection_tex_->getBuffer()->getRenderTarget();
    rtt->removeAllListeners();
    rtt->removeAllViewports();
    Ogre::TextureManager::getSingleton().remove(reflection_tex_);
  }

  if (refraction_tex_) {
    Ogre::RenderTarget *rtt = refraction_tex_->getBuffer()->getRenderTarget();
    rtt->removeAllListeners();
    rtt->removeAllViewports();
    Ogre::TextureManager::getSingleton().remove(refraction_tex_);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Init_(unsigned int tex_size) {
  using namespace Ogre;
  auto size = tex_size;
  auto &tex_manager = Ogre::TextureManager::getSingleton();

  // create our reflection & refraction render textures, and setup their render
  // targets
  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera = scene_->getCamera("Default");

  rcamera_ = camera;
  rcamera_->setAutoAspectRatio(false);

  reflection_tex_ =
      tex_manager.createManual("Reflection", RGN_DEFAULT, TEX_TYPE_2D, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  Ogre::RenderTarget *rtt1 = reflection_tex_->getBuffer()->getRenderTarget();
  Ogre::Viewport *vp1 = rtt1->addViewport(rcamera_.get());
  vp1->setOverlaysEnabled(false);
  vp1->setShadowsEnabled(false);
  // toggle reflection in camera
  rtt1->addListener(this);
  vp1->setVisibilityMask(SURFACE_MASK);

  refraction_tex_ =
      tex_manager.createManual("Refraction", RGN_DEFAULT, TEX_TYPE_2D, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  Ogre::RenderTarget *rtt2 = refraction_tex_->getBuffer()->getRenderTarget();
  Ogre::Viewport *vp2 = rtt2->addViewport(camera);
  vp2->setOverlaysEnabled(false);
  vp2->setShadowsEnabled(false);
  rtt2->addListener(this);
  vp2->setVisibilityMask(SUBMERGED_MASK);

  rcamera_->setAutoAspectRatio(true);
}

//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::SetPlane(Ogre::Plane plane) { plane_ = plane; }

}  // namespace glue

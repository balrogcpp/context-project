/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"

#include "ReflectionCamera.h"
#include "CameraMan.h"
#include "ContextManager.h"

namespace Context {
ReflectionCamera ReflectionCamera::CubeMapCameraSingleton;
//----------------------------------------------------------------------------------------------------------------------
ReflectionCamera *ReflectionCamera::GetSingletonPtr() {
  return &CubeMapCameraSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
ReflectionCamera &ReflectionCamera::GetSingleton() {
  return CubeMapCameraSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
bool ReflectionCamera::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  reflection_camera_->setFOVy(ogre_camera_->getFOVy());
  reflection_camera_->enableReflection(plane_);
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  reflection_camera_->disableReflection();
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::FreeCamera() {
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::RegPlane(Ogre::Plane plane) {
  plane_ = plane;
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::UnregPlane() {
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Setup() {
  // create our reflection & refraction render textures, and setup their render targets

  if (!reflection_camera_) {
    reflection_camera_ = ogre_scene_manager_->createCamera("CubeMapCamera");
    reflection_camera_->setProjectionType(Ogre::PT_PERSPECTIVE);
    reflection_camera_->setFOVy(ogre_camera_->getFOVy());
    reflection_camera_->setAspectRatio(ogre_camera_->getAspectRatio());
    reflection_camera_->setLodBias(0.2);
    reflection_camera_->setNearClipDistance(ogre_camera_->getNearClipDistance());
    reflection_camera_->setFarClipDistance(ogre_camera_->getFarClipDistance());
  }

  ogre_reflection_camera_node_ = ogre_camera_->getParentSceneNode()->createChildSceneNode();
  ogre_reflection_camera_node_->attachObject(reflection_camera_);

//  reflection_camera_ = ogre_camera_;

  if (!reflection_)
    reflection_ = Ogre::TextureManager::getSingleton().createManual("reflection",
                                                                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                    Ogre::TEX_TYPE_2D,
                                                                    1024,
                                                                    1024,
                                                                    0,
                                                                    Ogre::PF_R8G8B8,
                                                                    Ogre::TU_RENDERTARGET);

  Ogre::RenderTarget *rtt1 = reflection_->getBuffer()->getRenderTarget();

  const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  const Ogre::uint32 SURFACE_MASK = 0x00F;
  const Ogre::uint32 WATER_MASK = 0xF00;

  if (!rtt1->getViewport(0)) {
    Ogre::Viewport *vp = rtt1->addViewport(reflection_camera_);
    vp->setOverlaysEnabled(false);
    vp->setShadowsEnabled(false);
    // toggle reflection in camera
    rtt1->addListener(this);
    vp->setVisibilityMask(SURFACE_MASK);
  }

  if (!refraction_ && reflection_camera_ == ogre_camera_) {
    refraction_ = Ogre::TextureManager::getSingleton().createManual("refraction",
                                                                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                    Ogre::TEX_TYPE_2D,
                                                                    1,
                                                                    1,
                                                                    0,
                                                                    Ogre::PF_R8G8B8,
                                                                    Ogre::TU_RENDERTARGET);

    Ogre::RenderTarget *rtt2 = refraction_->getBuffer()->getRenderTarget();

    if (!rtt2->getViewport(0)) {
      Ogre::Viewport *vp = rtt2->addViewport(ogre_camera_);
      vp->setOverlaysEnabled(false);
      vp->setShadowsEnabled(false);
      // toggle reflection in camera
      rtt2->addListener(this);
      vp->setVisibilityMask(SUBMERGED_MASK);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Reset() {
//  reflection_planes_.clear();
}
}
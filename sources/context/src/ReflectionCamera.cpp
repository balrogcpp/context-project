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

#include "pcheader.hpp"

#include "ReflectionCamera.hpp"
#include "CameraMan.hpp"
#include "ContextManager.hpp"

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
//  for (const auto &it : reflection_planes_) {
//    ogre_camera_->enableReflection(Ogre::Plane(Ogre::Vector4(0, 1, 0, -5)));
    ogre_camera_->enableReflection(plane_);
//  }
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  ogre_camera_->disableReflection();
  ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::FreeCamera() {
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::RegPlane(Ogre::Plane plane) {
//  reflection_planes_.push_back(plane);
  plane_ = plane;
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::UnregPlane() {
//  reflection_planes_.clear();
//  reflection_planes_.(plane);
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Setup() {
  ogre_camera_->setAutoAspectRatio(false);
  // create our reflection & refraction render textures, and setup their render targets

  if (!reflection_)
    reflection_ = Ogre::TextureManager::getSingleton().createManual("reflection",
                                                                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                    Ogre::TEX_TYPE_2D,
                                                                    512,
                                                                    512,
                                                                    0,
                                                                    Ogre::PF_R8G8B8,
                                                                    Ogre::TU_RENDERTARGET);

  Ogre::RenderTarget *rtt = reflection_->getBuffer()->getRenderTarget();

  if (!rtt->getViewport(0)) {
    Ogre::Viewport *vp = rtt->addViewport(ogre_camera_);
    vp->setOverlaysEnabled(false);
    vp->setShadowsEnabled(false);
    // toggle reflection in camera
    rtt->addListener(this);
    vp->setVisibilityMask(0x000F);
  }

  ogre_camera_->setAutoAspectRatio(true);
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Reset() {
//  reflection_planes_.clear();
}
}
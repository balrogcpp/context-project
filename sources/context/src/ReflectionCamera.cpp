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
  for (const auto &it : reflection_planes_) {
    ogre_camera_->enableReflection(it);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  ogre_camera_->disableReflection();
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::FreeCamera() {
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

//  Ogre::MaterialManager::getSingleton()
//      .getByName("Examples/FresnelReflectionRefraction", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
//      ->getTechnique(0)
//      ->getPass(0)
//      ->getTextureUnitState(reflection->getName())
//      ->setTexture(reflection);

  Ogre::RenderTarget *rtt = reflection_->getBuffer()->getRenderTarget();
  Ogre::Viewport *vp = rtt->addViewport(ogre_camera_);

  // for refraction, only render submerged entities
  // for reflection, only render surface entities
  vp->setOverlaysEnabled(false);
  vp->setShadowsEnabled(false);

  // toggle reflection in camera
  rtt->addListener(this);

  ogre_camera_->setAutoAspectRatio(true);

//  // create our water plane mesh
//  mWaterPlane = Plane(Vector3::UNIT_Y, 0);
//  MeshManager::getSingleton().createPlane("water", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
//                                          mWaterPlane, 700, 1300, 10, 10, true, 1, 3, 5, Vector3::UNIT_Z);
//
//  // create a water entity using our mesh, give it the shader material, and attach it to the origin
//  mWater = mSceneMgr->createEntity("Water", "water");
//  mWater->setMaterialName("Examples/FresnelReflectionRefraction");
//  mSceneMgr->getRootSceneNode()->attachObject(mWater);
}
//----------------------------------------------------------------------------------------------------------------------
void ReflectionCamera::Reset() {

}
}
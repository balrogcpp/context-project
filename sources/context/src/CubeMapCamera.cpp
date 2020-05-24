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

#include "CubeMapCamera.hpp"
#include "CameraMan.hpp"
#include "ContextManager.hpp"

namespace Context {
CubeMapCamera CubeMapCamera::CubeMapCameraSingleton;
//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera *CubeMapCamera::GetSingletonPtr() {
  return &CubeMapCameraSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera &CubeMapCamera::GetSingleton() {
  return CubeMapCameraSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
bool CubeMapCamera::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  // point the camera in the right direction based on which face of the cubemap this is
  ogre_cube_camera_node_->setOrientation(Ogre::Quaternion::IDENTITY);
  ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

  if (evt.source == targets[0]) {
//    ogre_cube_camera_node_->yaw(Ogre::Degree(-90));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  } else if (evt.source == targets[1]) {
//    ogre_cube_camera_node_->yaw(Ogre::Degree(90));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  } else if (evt.source == targets[2]) {
//    ogre_cube_camera_node_->pitch(Ogre::Degree(90));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  } else if (evt.source == targets[3]) {
//    ogre_cube_camera_node_->pitch(Ogre::Degree(-90));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  } else if (evt.source == targets[4]) {
//    ogre_cube_camera_node_->pitch(Ogre::Degree(180));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  } else if (evt.source == targets[5]) {
//    ogre_cube_camera_node_->yaw(Ogre::Degree(180));
    ogre_cube_camera_node_->setOrientation(0, 1, 0, 0);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
}
//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::FreeCamera() {
  if (ogre_cube_camera_node_)
  {
    ogre_cube_camera_node_->detachAllObjects();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Setup() {
  if (!ogre_cube_camera_) {
    ogre_cube_camera_ = ogre_scene_manager_->createCamera("CubeMapCamera");
    ogre_cube_camera_->setProjectionType(Ogre::PT_PERSPECTIVE);
    ogre_cube_camera_->setFOVy(Ogre::Degree(90));
    ogre_cube_camera_->setAspectRatio(1);
    ogre_cube_camera_->setLodBias(0.2);
    ogre_cube_camera_->setNearClipDistance(1);
    ogre_cube_camera_->setFarClipDistance(10000);
  }

  ogre_cube_camera_node_ = ContextManager::GetSingleton().GetCameraMan()->GetCameraNode()->createChildSceneNode();
  ogre_cube_camera_node_->attachObject(ogre_cube_camera_);

  if (!dyncubemap) {
    dyncubemap = Ogre::TextureManager::getSingleton().createManual("dyncubemap",
                                                                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                   Ogre::TEX_TYPE_CUBE_MAP,
                                                                   128,
                                                                   128,
                                                                   0,
                                                                   Ogre::PF_R8G8B8,
                                                                   Ogre::TU_RENDERTARGET);
  }

  // assign our camera to all 6 render targets of the texture (1 for each direction)
  for (unsigned int i = 0; i < 6; i++) {
    targets[i] = dyncubemap->getBuffer(i)->getRenderTarget();

    if (!targets[i]->getViewport(0)) {
      Ogre::Viewport *vp = targets[i]->addViewport(ogre_cube_camera_);
      vp->setShadowsEnabled(false);
      vp->setVisibilityMask(0xF0);
      vp->setOverlaysEnabled(false);
      targets[i]->addListener(this);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Reset() {

}
}
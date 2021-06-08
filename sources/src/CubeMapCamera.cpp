// This source file is part of context-project
// Created by Andrew Vasiliev

#include "CubeMapCamera.h"

#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera::CubeMapCamera(Ogre::SceneNode *creator, unsigned int tex_size) { Init_(creator, tex_size); }

//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera::~CubeMapCamera() { Clear_(); }

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  // point the camera in the right direction based on which face of the cubemap
  // this is
  camera_node_->setOrientation(Ogre::Quaternion::IDENTITY);
  if (evt.source == targets_[0]) {
    camera_node_->yaw(Ogre::Degree(-90));
  } else if (evt.source == targets_[1]) {
    camera_node_->yaw(Ogre::Degree(90));
  } else if (evt.source == targets_[2]) {
    camera_node_->pitch(Ogre::Degree(90));
  } else if (evt.source == targets_[3]) {
    camera_node_->pitch(Ogre::Degree(-90));
  } else if (evt.source == targets_[5]) {
    camera_node_->yaw(Ogre::Degree(180));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Clear_() {
  if (cubemap_) {
    for (auto it : targets_) {
      it->removeAllListeners();
      it->removeAllViewports();
      it = nullptr;
    }

    Ogre::TextureManager::getSingleton().remove(cubemap_);
  }

  if (camera_) {
    scene_->destroyCamera(camera_);
    camera_ = nullptr;
  }

  camera_node_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Init_(Ogre::SceneNode *creator, unsigned int tex_size) {
  using namespace Ogre;
  auto size = tex_size;
  auto &tex_manager = Ogre::TextureManager::getSingleton();

  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *main_camera = scene_->getCamera("Default");
  camera_ = scene_->createCamera("CubeMapCamera");
  camera_->setProjectionType(Ogre::PT_PERSPECTIVE);
  camera_->setFOVy(Ogre::Degree(90.0));
  camera_->setAspectRatio(1.0);
  camera_->setLodBias(0.1);
  camera_->setNearClipDistance(main_camera->getNearClipDistance());
  camera_->setFarClipDistance(main_camera->getFarClipDistance());
  camera_node_ = creator->createChildSceneNode();
  camera_node_->setFixedYawAxis(false);
  camera_node_->attachObject(camera_);
  cubemap_ =
      tex_manager.createManual("dyncubemap", RGN_DEFAULT, TEX_TYPE_CUBE_MAP, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  // assign our camera to all 6 render targets of the texture (1 for each
  // direction)
  for (int i = 0; i < 6; i++) {
    targets_[i] = cubemap_->getBuffer(i)->getRenderTarget();
    Ogre::Viewport *vp = targets_[i]->addViewport(camera_);
    vp->setShadowsEnabled(false);
    vp->setOverlaysEnabled(false);
    targets_[i]->addListener(this);
  }
}

}  // namespace xio

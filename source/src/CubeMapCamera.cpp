// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "CubeMapCamera.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera::CubeMapCamera(Ogre::SceneNode *creator, unsigned int tex_size) { Init(creator, tex_size); }

//----------------------------------------------------------------------------------------------------------------------
CubeMapCamera::~CubeMapCamera() { Clear(); }

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  // point the camera in the right direction based on which face of the cubemap
  // this is
  camera_node->setOrientation(Ogre::Quaternion::IDENTITY);
  if (evt.source == targets[0]) {
    camera_node->yaw(Ogre::Degree(-90));
  } else if (evt.source == targets[1]) {
    camera_node->yaw(Ogre::Degree(90));
  } else if (evt.source == targets[2]) {
    camera_node->pitch(Ogre::Degree(90));
  } else if (evt.source == targets[3]) {
    camera_node->pitch(Ogre::Degree(-90));
  } else if (evt.source == targets[5]) {
    camera_node->yaw(Ogre::Degree(180));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Clear() {
  if (cubemap) {
    for (auto it : targets) {
      it->removeAllListeners();
      it->removeAllViewports();
      it = nullptr;
    }

    Ogre::TextureManager::getSingleton().remove(cubemap);
  }

  if (camera) {
    scene->destroyCamera(camera);
    camera = nullptr;
  }

  camera_node = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
void CubeMapCamera::Init(Ogre::SceneNode *creator, unsigned int tex_size) {
  using namespace Ogre;
  auto size = tex_size;
  auto &tex_manager = Ogre::TextureManager::getSingleton();

  scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *main_camera = scene->getCamera("Default");
  camera = scene->createCamera("CubeMapCamera");
  camera->setProjectionType(Ogre::PT_PERSPECTIVE);
  camera->setFOVy(Ogre::Degree(90.0));
  camera->setAspectRatio(1.0);
  camera->setLodBias(0.1);
  camera->setNearClipDistance(main_camera->getNearClipDistance());
  camera->setFarClipDistance(main_camera->getFarClipDistance());
  camera_node = creator->createChildSceneNode();
  camera_node->setFixedYawAxis(false);
  camera_node->attachObject(camera);
  cubemap =
      tex_manager.createManual("dyncubemap", RGN_DEFAULT, TEX_TYPE_CUBE_MAP, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  // assign our camera to all 6 render targets of the texture (1 for each
  // direction)
  for (int i = 0; i < 6; i++) {
    targets[i] = cubemap->getBuffer(i)->getRenderTarget();
    Ogre::Viewport *vp = targets[i]->addViewport(camera);
    vp->setShadowsEnabled(false);
    vp->setOverlaysEnabled(false);
    targets[i]->addListener(this);
  }
}

}  // namespace glue

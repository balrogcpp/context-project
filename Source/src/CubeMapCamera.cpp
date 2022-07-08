// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "CubeMapCamera.h"

using namespace std;

namespace Glue {

CubeMapCamera::CubeMapCamera(Ogre::SceneNode *ParentNode, unsigned int TextureSize) { Init(ParentNode, TextureSize); }

CubeMapCamera::~CubeMapCamera() { Clear(); }

void CubeMapCamera::preRenderTargetUpdate(const Ogre::RenderTargetEvent &Event) {
  // point the camera in the right direction based on which face of the cubemap
  // this is
  OgreCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
  if (Event.source == targets[0]) {
    OgreCameraNode->yaw(Ogre::Degree(-90));
  } else if (Event.source == targets[1]) {
    OgreCameraNode->yaw(Ogre::Degree(90));
  } else if (Event.source == targets[2]) {
    OgreCameraNode->pitch(Ogre::Degree(90));
  } else if (Event.source == targets[3]) {
    OgreCameraNode->pitch(Ogre::Degree(-90));
  } else if (Event.source == targets[5]) {
    OgreCameraNode->yaw(Ogre::Degree(180));
  }
}

void CubeMapCamera::postRenderTargetUpdate(const Ogre::RenderTargetEvent &Event) {}

void CubeMapCamera::Clear() {
  if (cubemap) {
    for (auto it : targets) {
      it->removeAllListeners();
      it->removeAllViewports();
      it = nullptr;
    }

    Ogre::TextureManager::getSingleton().remove(cubemap);
  }

  if (OgreCamera) {
    OgreSceneManager->destroyCamera(OgreCamera);
    OgreCamera = nullptr;
  }

  OgreCameraNode = nullptr;
}

void CubeMapCamera::Init(Ogre::SceneNode *creator, unsigned int tex_size) {
  using namespace Ogre;
  auto size = tex_size;
  auto &tex_manager = Ogre::TextureManager::getSingleton();

  OgreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *main_camera = OgreSceneManager->getCamera("Default");
  OgreCamera = OgreSceneManager->createCamera("CubeMapCamera");
  OgreCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
  OgreCamera->setFOVy(Ogre::Degree(90.0));
  OgreCamera->setAspectRatio(1.0);
  OgreCamera->setLodBias(0.1);
  OgreCamera->setNearClipDistance(main_camera->getNearClipDistance());
  OgreCamera->setFarClipDistance(main_camera->getFarClipDistance());
  OgreCameraNode = creator->createChildSceneNode();
  OgreCameraNode->setFixedYawAxis(false);
  OgreCameraNode->attachObject(OgreCamera);
  cubemap = tex_manager.createManual("dyncubemap", RGN_DEFAULT, TEX_TYPE_CUBE_MAP, size, size, 0, PF_R8G8B8, TU_RENDERTARGET);

  // assign our camera to all 6 render targets of the texture (1 for each
  // direction)
  for (int i = 0; i < 6; i++) {
    targets[i] = cubemap->getBuffer(i)->getRenderTarget();
    Ogre::Viewport *vp = targets[i]->addViewport(OgreCamera);
    vp->setShadowsEnabled(false);
    vp->setOverlaysEnabled(false);
    targets[i]->addListener(this);
  }
}

}  // namespace Glue

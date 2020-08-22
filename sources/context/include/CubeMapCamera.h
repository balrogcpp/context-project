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

#pragma once

#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>

namespace Context {

class CubeMapCamera final : public Ogre::RenderTargetListener, public Ogre::FrameListener {
 public:
//----------------------------------------------------------------------------------------------------------------------
  CubeMapCamera() {
    Init_();
  }

  virtual ~CubeMapCamera() {
    if (camera_node_) {
      camera_node_->detachAllObjects();
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    // point the camera in the right direction based on which face of the cubemap this is
    camera_node_->setOrientation(Ogre::Quaternion::IDENTITY);
    Ogre::Root::getSingleton().getSceneManager("Default")->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    if (evt.source == targets_[0]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    } else if (evt.source == targets_[1]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    } else if (evt.source == targets_[2]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    } else if (evt.source == targets_[3]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    } else if (evt.source == targets_[4]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    } else if (evt.source == targets_[5]) {
      camera_node_->setOrientation(0, 1, 0, 0);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    Ogre::Root::getSingleton().getSceneManager("Default")->
        setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  }
//----------------------------------------------------------------------------------------------------------------------
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) final { return true; };

 private:
//----------------------------------------------------------------------------------------------------------------------
  void Init_() {
    if (!camera_) {
      camera_ = Ogre::Root::getSingleton().getSceneManager("Default")->createCamera("CubeMapCamera");
      camera_->setProjectionType(Ogre::PT_PERSPECTIVE);
      camera_->setFOVy(Ogre::Degree(90));
      camera_->setAspectRatio(1);
      camera_->setLodBias(0.2);
      camera_->setNearClipDistance(1);
      camera_->setFarClipDistance(10000);
    }

//    camera_node_ = ContextManager::GetSingleton().GetCameraMan()->GetCameraNode()->createChildSceneNode();
    camera_node_->attachObject(camera_);

    if (!cubemap_) {
      cubemap_ = Ogre::TextureManager::getSingleton().createManual("dyncubemap",
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
      targets_[i] = cubemap_->getBuffer(i)->getRenderTarget();

      if (!targets_[i]->getViewport(0)) {
        Ogre::Viewport *vp = targets_[i]->addViewport(camera_);
        vp->setShadowsEnabled(false);
        vp->setVisibilityMask(0xF0);
        vp->setOverlaysEnabled(false);
        targets_[i]->addListener(this);
      }
    }
  }

  Ogre::Camera *camera_ = nullptr;
  Ogre::SceneNode *camera_node_ = nullptr;
  std::shared_ptr<Ogre::Texture> cubemap_;
  Ogre::RenderTarget *targets_[6] {nullptr};

 public:
  inline std::shared_ptr<Ogre::Texture> GetDyncubemap() const {
    return cubemap_;
  }
};
} //namespace Context

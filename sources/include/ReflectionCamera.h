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

#include <OgrePlane.h>
#include <OgreRenderTargetListener.h>

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
}

namespace xio {

class ReflectionCamera final : public Ogre::RenderTargetListener {
 public:
  ReflectionCamera() {
    Init_();
  }

  explicit ReflectionCamera(Ogre::Plane plane) {
    SetPlane(plane);
    Init_();
  }

  virtual ~ReflectionCamera() = default;

 public:
//----------------------------------------------------------------------------------------------------------------------
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    scene->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    camera_->setFOVy(scene->getCamera("Default")->getFOVy());
    camera_->enableReflection(plane_);
  }
//----------------------------------------------------------------------------------------------------------------------
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    Ogre::Root::getSingleton().getSceneManager("Default")->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    camera_->disableReflection();
  }

 private:
//----------------------------------------------------------------------------------------------------------------------
  void Init_() {
    // create our reflection & refraction render textures, and setup their render targets
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *camera = scene->getCamera("Default");

    if (!camera_) {
      camera_ = scene->createCamera("CubeMapCamera");
      camera_->setProjectionType(Ogre::PT_PERSPECTIVE);
      camera_->setFOVy(camera->getFOVy());
      camera_->setAspectRatio(camera->getAspectRatio());
      camera_->setLodBias(0.2);
      camera_->setNearClipDistance(camera->getNearClipDistance());
      camera_->setFarClipDistance(camera->getFarClipDistance());
    }

    ref_cam_node_ = camera->getParentSceneNode()->createChildSceneNode();
    ref_cam_node_->attachObject(camera_);

    if (!reflection_tex_)
      reflection_tex_ = Ogre::TextureManager::getSingleton().createManual("reflection",
                                                                          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                          Ogre::TEX_TYPE_2D,
                                                                          1024,
                                                                          1024,
                                                                          0,
                                                                          Ogre::PF_R8G8B8,
                                                                          Ogre::TU_RENDERTARGET);

    Ogre::RenderTarget *rtt1 = reflection_tex_->getBuffer()->getRenderTarget();

    const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
    const Ogre::uint32 SURFACE_MASK = 0x00F;
    const Ogre::uint32 WATER_MASK = 0xF00;

    if (!rtt1->getViewport(0)) {
      Ogre::Viewport *vp = rtt1->addViewport(camera_);
      vp->setOverlaysEnabled(false);
      vp->setShadowsEnabled(false);
      // toggle reflection in camera
      rtt1->addListener(this);
      vp->setVisibilityMask(SURFACE_MASK);
    }

    if (!refraction_tex_ && camera_ == camera) {
      refraction_tex_ = Ogre::TextureManager::getSingleton().createManual("refraction",
                                                                          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                          Ogre::TEX_TYPE_2D,
                                                                          1,
                                                                          1,
                                                                          0,
                                                                          Ogre::PF_R8G8B8,
                                                                          Ogre::TU_RENDERTARGET);

      Ogre::RenderTarget *rtt2 = refraction_tex_->getBuffer()->getRenderTarget();

      if (!rtt2->getViewport(0)) {
        Ogre::Viewport *vp = rtt2->addViewport(camera);
        vp->setOverlaysEnabled(false);
        vp->setShadowsEnabled(false);
        // toggle reflection in camera
        rtt2->addListener(this);
        vp->setVisibilityMask(SUBMERGED_MASK);
      }
    }
  }

  std::shared_ptr<Ogre::Texture> reflection_tex_;
  std::shared_ptr<Ogre::Texture> refraction_tex_;
  Ogre::Plane plane_;
  Ogre::Camera *camera_ = nullptr;
  Ogre::SceneNode *ref_cam_node_ = nullptr;

 public:
//----------------------------------------------------------------------------------------------------------------------
  void SetPlane(Ogre::Plane plane) {
    plane_ = plane;
  }

  std::shared_ptr<Ogre::Texture> GetReflectionTex() const {
    return reflection_tex_;
  }

  std::shared_ptr<Ogre::Texture> GetRefractionTex() const {
    return refraction_tex_;
  }
};
}
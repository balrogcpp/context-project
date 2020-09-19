//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#include <Ogre.h>

namespace xio {
class ReflectionCamera final : public Ogre::RenderTargetListener {
 public:
  explicit ReflectionCamera(Ogre::Plane plane, uint32_t tex_size) {
    SetPlane(plane);
    Init_(tex_size);
  }

  virtual ~ReflectionCamera() {
    Clear_();
  }

 public:
//----------------------------------------------------------------------------------------------------------------------
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    rcamera_->enableReflection(plane_);
  }
//----------------------------------------------------------------------------------------------------------------------
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final {
    rcamera_->disableReflection();
    scene_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  }

 private:
//----------------------------------------------------------------------------------------------------------------------
  void Clear_() {
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
  void Init_(uint32_t tex_size) {
    // create our reflection & refraction render textures, and setup their render targets
    scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *camera = scene_->getCamera("Default");

    rcamera_ = camera;
    rcamera_->setAutoAspectRatio(false);

    reflection_tex_ = Ogre::TextureManager::getSingleton().createManual("Reflection",
                                                                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                        Ogre::TEX_TYPE_2D,
                                                                        tex_size,
                                                                        tex_size,
                                                                        0,
                                                                        Ogre::PF_R8G8B8,
                                                                        Ogre::TU_RENDERTARGET);

    Ogre::RenderTarget *rtt1 = reflection_tex_->getBuffer()->getRenderTarget();
    Ogre::Viewport *vp1 = rtt1->addViewport(rcamera_);
    vp1->setOverlaysEnabled(false);
    vp1->setShadowsEnabled(false);
    // toggle reflection in camera
    rtt1->addListener(this);
    vp1->setVisibilityMask(SURFACE_MASK);

//    refraction_tex_ = Ogre::TextureManager::getSingleton().createManual("Refraction",
//                                                                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
//                                                                        Ogre::TEX_TYPE_2D,
//                                                                        tex_size,
//                                                                        tex_size,
//                                                                        0,
//                                                                        Ogre::PF_R8G8B8,
//                                                                        Ogre::TU_RENDERTARGET);
//
//    Ogre::RenderTarget *rtt2 = refraction_tex_->getBuffer()->getRenderTarget();
//    Ogre::Viewport *vp2 = rtt2->addViewport(camera);
//    vp2->setOverlaysEnabled(false);
//    vp2->setShadowsEnabled(false);
//    // toggle reflection in camera
//    rtt2->addListener(this);
//    vp2->setVisibilityMask(SUBMERGED_MASK);

    rcamera_->setAutoAspectRatio(true);
  }

  std::shared_ptr<Ogre::Texture> reflection_tex_;
  std::shared_ptr<Ogre::Texture> refraction_tex_;
  Ogre::Plane plane_;
  Ogre::Camera *rcamera_ = nullptr;
  Ogre::SceneNode *rcamera_node_ = nullptr;
  Ogre::SceneManager *scene_ = nullptr;

  const uint32_t SUBMERGED_MASK = 0x0F0;
  const uint32_t SURFACE_MASK = 0x00F;
  const uint32_t WATER_MASK = 0xF00;
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
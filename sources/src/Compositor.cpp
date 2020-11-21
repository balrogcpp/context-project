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

#include "pcheader.h"
#include "Compositor.h"
#include "Exception.h"

namespace xio {
class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() {
    ref_mat_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer");
    ref_mat_->load();

    ref_mat2_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer_alpha");
    ref_mat2_->load();

    techniques_.reserve(64);
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
                                        const Ogre::String &schemeName,
                                        Ogre::Material *originalMaterial,
                                        unsigned short lodIndex,
                                        const Ogre::Renderable *rend) final {
    auto *pass = originalMaterial->getTechnique(0)->getPass(0);
    int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
    Ogre::Technique *gBufferTech = originalMaterial->createTechnique();

    if (pass->getNumTextureUnitStates() > 0 && alpha_rejection > 0) {
      gBufferTech->setSchemeName(schemeName);
      Ogre::Pass *gbufPass = gBufferTech->createPass();
      *gbufPass = *ref_mat2_->getTechnique(0)->getPass(0);

      auto texture_albedo = pass->getTextureUnitState("Albedo");
      if (texture_albedo) {
        std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();
        auto *texPtr3 = gBufferTech->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureAddressingMode(Ogre::TextureAddressingMode::TAM_CLAMP);
          texPtr3->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
          texPtr3->setTextureName(texture_name);
        }
      }
    } else {
      gBufferTech->setSchemeName(schemeName);
      Ogre::Pass *gbufPass = gBufferTech->createPass();
      *gbufPass = *ref_mat_->getTechnique(0)->getPass(0);
    }

    techniques_.push_back(gBufferTech->getPass(0)->getVertexProgramParameters());
    return gBufferTech;
  }

//----------------------------------------------------------------------------------------------------------------------
  void Update(Ogre::Matrix4 mvp) {
    for (auto &it : techniques_) {
      it->setNamedConstant("cWorldViewProjPrev", mvp);
    }
  }

//----------------------------------------------------------------------------------------------------------------------
  void Clear() {
    techniques_.clear();
    techniques_.shrink_to_fit();
  }
 private:
  Ogre::MaterialPtr ref_mat_;
  Ogre::MaterialPtr ref_mat2_;
  std::vector<Ogre::GpuProgramParametersSharedPtr> techniques_;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
Compositor::Compositor() {
  effects_["bloom"] = false;
  effects_["ssao"] = false;
  effects_["motion"] = false;
  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  camera_ = scene_->getCamera("Default");
  viewport_ = camera_->getViewport();
}
Compositor::~Compositor() noexcept {}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Loop(float time) {
  if (effects_["motion"]) {
    mvp_prev_ = mvp_;
    mvp_ = camera_->getProjectionMatrixWithRSDepth() * camera_->getViewMatrix();
    gbuff_handler_->Update(mvp_prev_);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Clean() {
  if (effects_["motion"]) {
    gbuff_handler_->Clear();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {

  if (effects_["ssao"] || effects_["motion"]) {
    gbuff_handler_ = new GBufferSchemeHandler();
    Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_, "GBuffer");
  }

  if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Main"))
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Main", true);
  else
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Main compositor\n");

  if (effects_["ssao"] || effects_["motion"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "GBuffer"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "GBuffer", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add ShadowReceiver compositor\n");
  }

  if (effects_["bloom"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Bloom"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Bloom", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Bloom compositor\n");

    for (int i = 0; i < 1; i++) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "FilterY/Bloom"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "FilterY/Bloom", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "FilterX/Bloom"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "FilterX/Bloom", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }
  }

  if (effects_["ssao"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Ssao"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Ssao", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

    for (int i = 0; i < 1; i++) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "FilterY/Ssao"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "FilterY/Ssao", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "FilterX/Ssao"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "FilterX/Ssao", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }
  }

  if (effects_["ssao"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Modulate/Ssao"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Modulate/Ssao", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  }

  if (effects_["motion"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "MotionBlur"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "MotionBlur", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  }

  if (effects_["bloom"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Modulate/Bloom"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Modulate/Bloom", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  }

  if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Output"))
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Output", true);
  else
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
}
}
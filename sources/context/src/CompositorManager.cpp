//
// Created by balrog on 07.07.19.
//
#include "pcheader.hpp"

#include "CompositorManager.hpp"
#include "Exception.hpp"
#include "ConfigManager.hpp"
#include "ShaderResolver.hpp"
#include "DotSceneLoaderB.hpp"

namespace Context {

/** Class for handling materials who did not specify techniques for rendering
 *  themselves into the GBuffer.
 */
class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() {
    mGBufRefMat = Ogre::MaterialManager::getSingleton().getByName("SSAO/GBuffer");
    Ogre::RTShader::ShaderGenerator::getSingleton().validateMaterial("GBuffer",
                                                                     "SSAO/GBuffer",
                                                                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mGBufRefMat->load();
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
                                                const Ogre::String &schemeName,
                                                Ogre::Material *originalMaterial,
                                                unsigned short lodIndex,
                                                const Ogre::Renderable *rend) final {
    Ogre::Technique *gBufferTech = originalMaterial->createTechnique();
    gBufferTech->setSchemeName(schemeName);
    Ogre::Pass *gbufPass = gBufferTech->createPass();
    *gbufPass = *mGBufRefMat->getTechnique(0)->getPass(0);
    if (originalMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0) {
      auto *texPtr2 = gbufPass->getTextureUnitState("BaseColor");
      texPtr2->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
      texPtr2->setTextureFiltering(Ogre::TFO_NONE);
      auto texture_name = originalMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
      texPtr2->setTextureName(texture_name);
    }

    return gBufferTech;
  }

 private:
  Ogre::MaterialPtr mGBufRefMat;
};

class ShadowReceiverSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  ShadowReceiverSchemeHandler() {
    mGBufRefMat = Ogre::MaterialManager::getSingleton().getByName("PSSM/ShadowReceiver");
    Ogre::RTShader::ShaderGenerator::getSingleton().validateMaterial("ShadowReceiver",
                                                                     "PSSM/ShadowReceiver",
                                                                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mGBufRefMat->load();

    mGBufRefMat2 = Ogre::MaterialManager::getSingleton().getByName("PSSM/NoShadow");
    Ogre::RTShader::ShaderGenerator::getSingleton().validateMaterial("ShadowReceiver",
                                                                     "PSSM/NoShadow",
                                                                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mGBufRefMat2->load();
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
                                                const Ogre::String &schemeName,
                                                Ogre::Material *originalMaterial,
                                                unsigned short lodIndex,
                                                const Ogre::Renderable *rend) final {
    auto texture_name = originalMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();

    if (originalMaterial->getReceiveShadows()) {

      Ogre::Technique *gBufferTech = originalMaterial->createTechnique();
      gBufferTech->setSchemeName(schemeName);
      Ogre::Pass *gbufPass = gBufferTech->createPass();
      *gbufPass = *mGBufRefMat->getTechnique(0)->getPass(0);

      auto *texPtr2 = gbufPass->getTextureUnitState("BaseColor");
      texPtr2->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
      texPtr2->setTextureFiltering(Ogre::TFO_NONE);
      texPtr2->setTextureName(texture_name);

      return gBufferTech;
    } else {
      Ogre::Technique *gBufferTech = originalMaterial->createTechnique();
      gBufferTech->setSchemeName(schemeName);
      Ogre::Pass *gbufPass = gBufferTech->createPass();
      *gbufPass = *mGBufRefMat2->getTechnique(0)->getPass(0);

      auto *texPtr2 = gbufPass->getTextureUnitState("BaseColor");
      texPtr2->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
      texPtr2->setTextureFiltering(Ogre::TFO_NONE);
      texPtr2->setTextureName(texture_name);

      return gBufferTech;

    }

  }

 private:
  Ogre::MaterialPtr mGBufRefMat;
  Ogre::MaterialPtr mGBufRefMat2;
};

CompositorManager CompositorManager::CompositorManagerSingleton;

//----------------------------------------------------------------------------------------------------------------------
CompositorManager *CompositorManager::GetSingletonPtr() {
  return &CompositorManagerSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
CompositorManager &CompositorManager::GetSingleton() {
  return CompositorManagerSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {

}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {

}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::Setup() {
  ConfigManager::Assign(graphics_shadows_enable_, "graphics_shadows_enable");
  ConfigManager::Assign(compositor_use_bloom_, "compositor_use_bloom");
  ConfigManager::Assign(compositor_use_ssao_, "compositor_use_ssao");
  ConfigManager::Assign(compositor_use_blur_, "compositor_use_blur");
  ConfigManager::Assign(compositor_use_hdr_, "compositor_use_hdr");
  ConfigManager::Assign(compositor_use_moution_blur_, "compositor_use_moution_blur");

  /// Motion blur effect
  Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
      "Motion Blur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
  );
  {
    Ogre::CompositionTechnique *t = comp3->createTechnique();
    {
      Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
      def->width = 0;
      def->height = 0;
      def->formatList.push_back(Ogre::PF_R8G8B8);
    }
    {
      Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
      def->width = 0;
      def->height = 0;
      def->formatList.push_back(Ogre::PF_R8G8B8);
    }
    {
      Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
      def->width = 0;
      def->height = 0;
      def->formatList.push_back(Ogre::PF_R8G8B8);
    }
    /// Render scene
    {
      Ogre::CompositionTargetPass *tp = t->createTargetPass();
      tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
      tp->setOutputName("scene");
    }
    /// Initialisation pass for sum texture
    {
      Ogre::CompositionTargetPass *tp = t->createTargetPass();
      tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
      tp->setOutputName("sum");
      tp->setOnlyInitial(true);
    }
    /// Do the motion blur
    {
      Ogre::CompositionTargetPass *tp = t->createTargetPass();
      tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
      tp->setOutputName("temp");
      { Ogre::CompositionPass *pass = tp->createPass(Ogre::CompositionPass::PT_RENDERQUAD);
        pass->setMaterialName("Ogre/Compositor/Combine");
        pass->setInput(0, "scene");
        pass->setInput(1, "sum");
      }
    }
    /// Copy back sum texture
    {
      Ogre::CompositionTargetPass *tp = t->createTargetPass();
      tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
      tp->setOutputName("sum");
      { Ogre::CompositionPass *pass = tp->createPass(Ogre::CompositionPass::PT_RENDERQUAD);
        pass->setMaterialName("Ogre/Compositor/Copyback");
        pass->setInput(0, "temp");
      }
    }
    /// Display result
    {
      Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
      tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
      { Ogre::CompositionPass *pass = tp->createPass(Ogre::CompositionPass::PT_RENDERQUAD);
        pass->setMaterialName("Ogre/Compositor/MotionBlur");
        pass->setInput(0, "sum");
      }
    }
  }

  if(ConfigManager::GetSingleton().GetBool("graphics_shadows_enable"))
  {
//    shadow_receiever_scheme_handler_ = std::make_unique<ShadowReceiverSchemeHandler>();
//    Ogre::MaterialManager::getSingleton().addListener(shadow_receiever_scheme_handler_.get(), "ShadowReceiver");

    ssaog_buffer_scheme_handler_ = std::make_unique<GBufferSchemeHandler>();
    Ogre::MaterialManager::getSingleton().addListener(ssaog_buffer_scheme_handler_.get(), "GBuffer");

    if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Main"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Main", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add ShadowReceiver compositor\n");

    if (compositor_use_bloom_) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Bloom"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Bloom", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }

    if (compositor_use_ssao_) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/SSAO"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/SSAO", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }

    if (compositor_use_blur_) {
      for (int i = 0; i < 1; i++) {
        if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Filter1"))
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Filter1", true);
        else
          Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

        if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Filter2"))
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Filter2", true);
        else
          Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
      }
    }

    if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Modulate"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Modulate", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");

    if (compositor_use_moution_blur_) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Motion Blur"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Motion Blur", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
    }

    if (compositor_use_hdr_) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/HDR"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/HDR", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
    } else {
    if (Ogre::CompositorManager::getSingleton().addCompositor(ogre_viewport_, "Context/Output"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, "Context/Output", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::Reset() {

}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::SetCompositor(const std::string &compositor_) {
  if (compositor_ != "none") {
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, current_post_, false);
    current_compositor_ = compositor_;
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, current_post_, true);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CompositorManager::SetPost(const std::string &post_) {
  if (post_ != "none") {
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, current_post_, false);
    current_post_ = post_;
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(ogre_viewport_, current_post_, true);
  }
}

} //namespace Context

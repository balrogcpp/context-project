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
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>

namespace xio {
class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() {
    mGBufRefMat = Ogre::MaterialManager::getSingleton().getByName("Context/gbuffer");
    mGBufRefMat->load();

    mGBufRefMat2 = Ogre::MaterialManager::getSingleton().getByName("Context/gbuffer_alpha");
    mGBufRefMat2->load();
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
      *gbufPass = *mGBufRefMat2->getTechnique(0)->getPass(0);

      auto texture_albedo = pass->getTextureUnitState("Albedo");
      if (texture_albedo) {
        std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();
        auto *texPtr3 = gBufferTech->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureAddressingMode(Ogre::TextureAddressingMode::TAM_CLAMP);
          texPtr3->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
          texPtr3->setTextureName(texture_name);
        }
      }
    }
    else {
      gBufferTech->setSchemeName(schemeName);
      Ogre::Pass *gbufPass = gBufferTech->createPass();
      *gbufPass = *mGBufRefMat->getTechnique(0)->getPass(0);
    }

    return gBufferTech;
  }

 private:
  Ogre::MaterialPtr mGBufRefMat;
  Ogre::MaterialPtr mGBufRefMat2;
};
}
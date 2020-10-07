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
#include "CompositorHelper.h"

namespace xio {
Compositor::~Compositor() noexcept {}
//----------------------------------------------------------------------------------------------------------------------
Compositor::Compositor() {
  effects_["bloom"] = false;
  effects_["ssao"] = false;
  effects_["blur"] = false;
  camera_ = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  viewport_ = camera_->getViewport();
}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Loop(float time) {
//  if (effects_["blur"]) {
//    mvp_prev_ = mvp_;
//    mvp_ = camera_->getProjectionMatrix() * camera_->getViewMatrix();
//    sbuff_handler_->Update(mvp_prev_);
//  }
}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {
  if (effects_["ssao"]) {
    gbuff_handler_ = new GBufferSchemeHandler();
    Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_, "GBuffer");
  }

  if (Ogre::CompositorManager::getSingleton().addCompositor(viewport_, "Main"))
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport_, "Main", true);
  else
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Main compositor\n");

  if (effects_["ssao"]) {
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
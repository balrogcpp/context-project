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
#include "CompositorHelpers.h"

namespace xio {
Compositor::~Compositor() noexcept {}
//----------------------------------------------------------------------------------------------------------------------
Compositor::Compositor() {
  effects_["bloom"] = false;
  effects_["ssao"] = false;
}
//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {
  if (effects_["ssao"]) {
    gbuff_handler_ = new GBufferSchemeHandler();
    Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_, "GBuffer");
  }
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *camera = scene->getCamera("Default");
    auto *viewport = camera->getViewport();

  if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/Main"))
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/Main", true);
  else
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Context/Main compositor\n");

  if (effects_["ssao"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/GBuffer"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/GBuffer", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add ShadowReceiver compositor\n");
  }

  if (effects_["bloom"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/Bloom"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/Bloom", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Bloom compositor\n");

    for (int i = 0; i < 1; i++) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/FilterY/Bloom"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/FilterY/Bloom", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/FilterX/Bloom"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/FilterX/Bloom", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }
  }

  if (effects_["ssao"]) {
    if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/SSAO"))
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/SSAO", true);
    else
      Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

    for (int i = 0; i < 1; i++) {
      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/FilterY/SSAO"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/FilterY/SSAO", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

      if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Context/FilterX/SSAO"))
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Context/FilterX/SSAO", true);
      else
        Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
    }
  }

  std::string modulate_compositor = "Context/Modulate";
  modulate_compositor += effects_["bloom"] ? "/Bloom" : "";
  modulate_compositor += effects_["ssao"] ? "/SSAO" : "";

  if (Ogre::CompositorManager::getSingleton().addCompositor(viewport, modulate_compositor))
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, modulate_compositor, true);
  else
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
}
}
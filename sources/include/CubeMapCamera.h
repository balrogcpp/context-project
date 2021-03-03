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
#include <array>

namespace xio {
class CubeMapCamera final : public Ogre::RenderTargetListener {
 public:
//----------------------------------------------------------------------------------------------------------------------
  CubeMapCamera(Ogre::SceneNode *creator, unsigned int tex_size);
  virtual ~CubeMapCamera();
  //----------------------------------------------------------------------------------------------------------------------
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;

 private:
  void Clear_();
  void Init_(Ogre::SceneNode *creator, unsigned int tex_size);

  Ogre::SceneManager *scene_;
  Ogre::Camera *camera_;
  Ogre::SceneNode *camera_node_;
  std::shared_ptr<Ogre::Texture> cubemap_;
  std::array<Ogre::RenderTarget*, 6> targets_;

 public:
  std::shared_ptr<Ogre::Texture> GetDyncubemap() const;
};
}

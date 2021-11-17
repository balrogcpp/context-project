// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <OgreRenderTargetListener.h>
#include <array>

namespace Ogre {
class SceneManager;
class SceneNode;
class Camera;
class Texture;
}  // namespace Ogre

namespace glue {

class CubeMapCamera final : public Ogre::RenderTargetListener {
 public:
  CubeMapCamera(Ogre::SceneNode *creator, unsigned int tex_size);
  virtual ~CubeMapCamera();

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;

  std::shared_ptr<Ogre::Texture> cubemap_;

 private:
  void Clear_();
  void Init_(Ogre::SceneNode *creator, unsigned int tex_size);

  Ogre::SceneManager *scene_ = nullptr;
  Ogre::Camera *camera_ = nullptr;
  Ogre::SceneNode *camera_node_ = nullptr;
  std::array<Ogre::RenderTarget *, 6> targets_{nullptr};
};

}  // namespace glue

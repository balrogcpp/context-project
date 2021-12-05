// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreRenderTargetListener.h>
#include <array>

namespace Ogre {
class SceneManager;
class SceneNode;
class Camera;
class Texture;
}  // namespace Ogre

namespace Glue {

class CubeMapCamera final : public Ogre::RenderTargetListener {
 public:
  CubeMapCamera(Ogre::SceneNode *creator, unsigned int tex_size);
  virtual ~CubeMapCamera();

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;

  std::shared_ptr<Ogre::Texture> cubemap;

 protected:
  void Clear();
  void Init(Ogre::SceneNode *creator, unsigned int tex_size);

  Ogre::SceneManager *scene = nullptr;
  Ogre::Camera *camera = nullptr;
  Ogre::SceneNode *camera_node = nullptr;
  std::array<Ogre::RenderTarget *, 6> targets;
};

}  // namespace glue

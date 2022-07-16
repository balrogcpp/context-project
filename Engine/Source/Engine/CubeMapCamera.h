// This source file is part of Glue Engine. Created by Andrey Vasiliev

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
  CubeMapCamera(Ogre::SceneNode *ParentNode, unsigned int TextureSize);
  virtual ~CubeMapCamera();

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &Event) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &Event) override;

  std::shared_ptr<Ogre::Texture> cubemap;

 protected:
  void Clear();
  void Init(Ogre::SceneNode *creator, unsigned int tex_size);

  Ogre::SceneManager *OgreSceneManager = nullptr;
  Ogre::Camera *OgreCamera = nullptr;
  Ogre::SceneNode *OgreCameraNode = nullptr;
  std::array<Ogre::RenderTarget *, 6> targets;
};

}  // namespace Glue

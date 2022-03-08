// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include <OgrePlane.h>
#include <OgreRenderTargetListener.h>
#include <vector>

namespace Ogre {
class Texture;
class SceneManager;
class Camera;
}  // namespace Ogre

namespace Glue {
class ReflectionCamera final : public Ogre::RenderTargetListener {
 public:
  ReflectionCamera(Ogre::Plane plane, unsigned int tex_size);
  virtual ~ReflectionCamera();

  void SetPlane(Ogre::Plane plane);

  const uint32_t SUBMERGED_MASK = 0x0F0;
  const uint32_t SURFACE_MASK = 0x00F;
  const uint32_t WATER_MASK = 0xF00;
  std::shared_ptr<Ogre::Texture> reflection_texture;
  std::shared_ptr<Ogre::Texture> refraction_texture;

 protected:
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;
  void Clear();

  Ogre::Plane plane;
  Ogre::Camera* rcamera;
  Ogre::SceneManager* scene;
};

}  // namespace Glue

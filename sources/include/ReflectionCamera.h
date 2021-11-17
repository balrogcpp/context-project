// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <OgreRenderTargetListener.h>
#include <OgrePlane.h>
#include <vector>

namespace Ogre {
class Texture;
class SceneManager;
class Camera;
}

namespace glue {
class ReflectionCamera final : public Ogre::RenderTargetListener {
 public:
  ReflectionCamera(Ogre::Plane plane, unsigned int tex_size);

  virtual ~ReflectionCamera();

  void SetPlane(Ogre::Plane plane);

  const uint32_t SUBMERGED_MASK = 0x0F0;
  const uint32_t SURFACE_MASK = 0x00F;
  const uint32_t WATER_MASK = 0xF00;
  std::shared_ptr<Ogre::Texture> reflection_tex_;
  std::shared_ptr<Ogre::Texture> refraction_tex_;

 protected:
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void Clear_();
  void Init_(unsigned int tex_size);

  Ogre::Plane plane_;
  view_ptr<Ogre::Camera> rcamera_;
  view_ptr<Ogre::SceneManager> scene_;
};

}  // namespace glue

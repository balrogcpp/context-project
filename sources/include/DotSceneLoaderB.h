// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <OgreSceneLoader.h>
#include <OgreVector4.h>

#include <string>
#include <vector>

#include "CameraMan.h"
#include "CubeMapCamera.h"
#include "Input.h"
#include "Landscape.h"
#include "ReflectionCamera.h"
#include "Scene.h"
#include "Singleton.h"
#include "System.h"
#include "VegetationSystem.h"
#include "view_ptr.h"

namespace pugi {
class xml_node;
}

namespace Ogre {
class SceneManager;
class SceneNode;
class TerrainGroup;
class TerrainGlobalOptions;
class VertexDeclaration;
}  // namespace Ogre

class SinbadCharacterController;

namespace xio {
class CameraMan;
class Config;
class RenderSystem;
class PhysicsSystem;
class AudioSystem;
class Overlay;

std::string GetAttrib(const pugi::xml_node &xml_node, const std::string &attrib,
                      const std::string &defaultValue = "");

float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib,
                    float defaultValue = 0);

int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib,
                 int defaultValue = 0);

bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib,
                   bool defaultValue = false);

Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node);

Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node);

Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node);

Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node);

Ogre::ColourValue ParseColour(pugi::xml_node &xml_node);

class DotSceneLoaderB final : public System,
                              public Ogre::SceneLoader,
                              public Singleton<DotSceneLoaderB> {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Cleanup() override;
  void Update(float time) override;

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name,
            Ogre::SceneNode *root_node) override;
  float GetHeigh(float x, float z);

  static Landscape &GetTerrain();
  CameraMan &GetCamera() const;
  static VegetationSystem &GetForest();

 private:
  void ProcessScene_(pugi::xml_node &xml_root);
  void ProcessNodes_(pugi::xml_node &xml_node);
  void ProcessExternals_(pugi::xml_node &xml_node);
  void ProcessEnvironment_(pugi::xml_node &xml_node);
  void ProcessUserData_(pugi::xml_node &xml_node,
                        Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight_(pugi::xml_node &xml_node,
                     Ogre::SceneNode *parent = nullptr);
  void ProcessCamera_(pugi::xml_node &xml_node,
                      Ogre::SceneNode *parent = nullptr);
  void ProcessNode_(pugi::xml_node &xml_node,
                    Ogre::SceneNode *parent = nullptr);
  void ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessParticleSystem_(pugi::xml_node &xml_node,
                              Ogre::SceneNode *parent);
  void ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessPlane_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessForest_(pugi::xml_node &xml_node);
  void ProcessLandscape_(pugi::xml_node &xml_node);
  void ProcessFog_(pugi::xml_node &xml_node);
  void ProcessSkyBox_(pugi::xml_node &xml_node);
  void ProcessSkyDome_(pugi::xml_node &xml_node);
  void ProcessSkyPlane_(pugi::xml_node &xml_node);
  void ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light);

  std::unique_ptr<ReflectionCamera> rcamera_;
  std::unique_ptr<CubeMapCamera> ccamera_;
  std::unique_ptr<CameraMan> camera_;
  std::unique_ptr<Scene> scene_;

  view_ptr<Ogre::SceneManager> ogre_scene_;
  view_ptr<Ogre::Root> root_;
  view_ptr<Ogre::SceneNode> root_node_;
  view_ptr<Ogre::SceneNode> attach_node_;
  std::string group_name_ = Ogre::RGN_DEFAULT;

  std::unique_ptr<SinbadCharacterController> sinbad_;

  static inline std::unique_ptr<Landscape> terrain_;
  static inline std::unique_ptr<VegetationSystem> forest_;
};

}  // namespace xio

/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Component.h"
#include "Terrain.h"
#include "Field.h"
#include "ReflectionCamera.h"
#include "CubeMapCamera.h"
#include "Input.h"
#include <OgreSceneLoader.h>
#include <OgreVector4.h>

#include <vector>
#include <string>

namespace pugi {
class xml_node;
}

namespace Ogre {
class SceneManager;
class SceneNode;
class Terrain;
class TerrainGroup;
class TerrainGlobalOptions;
class VertexDeclaration;
}

namespace xio {
class Camera;
class JsonConfigurator;
class YamlConfigurator;
class Renderer;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;

class DotSceneLoaderB final : public Component, public Ogre::SceneLoader {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Create() final {}
  void Clear() final {}
  void Clean() final;
  void Loop(float time) final {}

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) final;
  void Load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node);
  float GetHeigh(float x, float z);
  void LocateComponents(YamlConfigurator *conf,
                        xio::InputSequencer *io,
                        Renderer *renderer,
                        Physics *physics,
                        Sound *sounds,
                        Overlay *overlay) {
    conf_ = conf;
    io_ = io;
    renderer_ = renderer;
    physics_ = physics;
    sounds_ = sounds;
    overlay_ = overlay;
  }

 private:
  void ProcessScene_(pugi::xml_node &xml_root);
  void ProcessNodes_(pugi::xml_node &xml_node);
  void ProcessExternals_(pugi::xml_node &xml_node);
  void ProcessEnvironment_(pugi::xml_node &xml_node);
  void ProcessUserData_(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessCamera_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessNode_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessParticleSystem_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessPlane_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessForest_(pugi::xml_node &xml_node);
  void ProcessFog_(pugi::xml_node &xml_node);
  void ProcessSkyBox_(pugi::xml_node &xml_node);
  void ProcessSkyDome_(pugi::xml_node &xml_node);
  void ProcessSkyPlane_(pugi::xml_node &xml_node);
  void ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light);

  std::unique_ptr<ReflectionCamera> rcamera_;
  std::unique_ptr<CubeMapCamera> cmcamera_;
  static std::unique_ptr<Terrain> terrain_;
  static std::unique_ptr<Field> forest_;
  std::unique_ptr<Camera> camera_;
  Ogre::SceneManager *scene_manager_ = nullptr;
  Ogre::SceneNode *attach_node_ = nullptr;
  std::string group_name_;

  YamlConfigurator *conf_ = nullptr;
  Renderer *renderer_ = nullptr;
  Physics *physics_ = nullptr;
  Sound *sounds_ = nullptr;
  Overlay *overlay_ = nullptr;
  xio::InputSequencer *io_ = nullptr;
};
}
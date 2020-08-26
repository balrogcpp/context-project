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
#include "ReflectionCamera.h"
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

namespace Context {
class CameraMan;
class JsonConfigurator;
class YamlConfigurator;
class Render;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;

class DotSceneLoaderB final : public Component, public Ogre::SceneLoader {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) final;
  void Load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node);

  float GetHeigh(float x, float z);

 private:
  void ProcessScene_(pugi::xml_node &xml_root);
  void ProcessNodes_(pugi::xml_node &xml_node);
  void ProcessExternals_(pugi::xml_node &xml_node);
  void ProcessEnvironment_(pugi::xml_node &xml_node);
  void ProcessTerrainGroup_(pugi::xml_node &xml_node);
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

  void CreateTerrainHeightfieldShape(int size,
                                     float *data,
                                     const float &min_height,
                                     const float &max_height,
                                     const Ogre::Vector3 &position,
                                     const float &scale);
  void GetTerrainImage(bool flipX, bool flipY, Ogre::Image &ogre_image, const std::string &filename);
  void DefineTerrain(long x, long y, bool flat, const std::string &filename);
  void InitBlendMaps(Ogre::Terrain *terrain, int layer, const std::string &image);

 private:
  std::unique_ptr<ReflectionCamera> rcamera_;
  std::shared_ptr<CameraMan> camera_man_;
  Ogre::SceneManager *scene_manager_ = nullptr;
  Ogre::SceneNode *attach_node_ = nullptr;
  std::string group_name_;
  std::shared_ptr<Ogre::TerrainGroup> ogre_terrain_group_;

  YamlConfigurator *conf_ = nullptr;
  Render *renderer_ = nullptr;
  Physics *physics_ = nullptr;
  Sound *sounds_ = nullptr;
  Overlay *overlay_ = nullptr;
  DotSceneLoaderB *loader_ = nullptr;
  io::InputSequencer *io_ = nullptr;

  bool lod_generator_enable_ = false;
  bool physics_enable_ = true;

  bool terrain_cast_shadows_ = false;
  bool terrain_raybox_calculation_ = true;
  bool terrain_generator_b_ = false;
  bool terrain_created_ = false;

 public:
  void GetComponents(YamlConfigurator *conf,
                     io::InputSequencer *io,
                     Render *renderer,
                     Physics *physics,
                     Sound *sounds,
                     Overlay *overlay,
                     DotSceneLoaderB *loader) {
    conf_ = conf;
    io_ = io;
    renderer_ = renderer;
    physics_ = physics;
    sounds_ = sounds;
    overlay_ = overlay;
    loader_ = loader;
  }
}; //class DotSceneLoaderB
} //namespace Context

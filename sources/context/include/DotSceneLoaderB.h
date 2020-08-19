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

#include "Manager.h"

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
class TerrainPaging;
class ShadowCameraSetup;
class VertexDeclaration;
}

namespace Ogre {
class HlmsManager;
class PbsMaterial;
}

namespace Context {

class DotSceneLoaderB final : public Manager, public Ogre::SceneLoader {
 public:
  static DotSceneLoaderB &GetSingleton() {
    static DotSceneLoaderB singleton;
    return singleton;
  }

  DotSceneLoaderB() {
//    SetupGlobal();
//    Setup();
  }

  virtual ~DotSceneLoaderB() {
//    ResetGlobal();
//    Reset();
  }

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) final;
  void Load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node);

  void Setup() final;
  void Reset() final;

  float GetHeigh(float x, float z);

  //utils
  static void UpdatePbrParams(Ogre::MaterialPtr material);
  static void UpdatePbrParams(const std::string &material);

  static void UpdatePbrShadowReceiver(Ogre::MaterialPtr material);
  static void UpdatePbrShadowReceiver(const std::string &material);

  static void UpdatePbrShadowCaster(Ogre::MaterialPtr material);
  static void UpdatePbrShadowCaster(const std::string &material);

  static void UpdateForestParams(Ogre::MaterialPtr material);
  static void UpdateForestParams(const std::string &material);

  static void EnsureHasTangents(Ogre::MeshPtr mesh_ptr);
  static bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration);

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
  Ogre::SceneManager *scene_manager_ = nullptr;
  Ogre::SceneNode *attach_node_ = nullptr;
  std::string group_name_;
  std::shared_ptr<Ogre::TerrainGroup> ogre_terrain_group_;
  std::vector<float> heigh_data_;
  Ogre::Vector4 terrain_box_;

  bool lod_generator_enable_ = false;
  bool physics_enable_ = true;

  bool terrain_cast_shadows_ = false;
  bool terrain_raybox_calculation_ = true;
  bool terrain_save_terrains_ = false;
  bool terrain_created_ = false;
  bool legacy_terrain_ = true;
  bool terrain_generator_b_ = false;
}; //class DotSceneLoaderB
} //namespace Context

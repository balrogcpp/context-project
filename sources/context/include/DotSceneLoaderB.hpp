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

#include "ManagerCommon.hpp"

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

struct Item {
  Ogre::Entity *entity_;
  btRigidBody *rigid_body_;
};

class DotSceneLoaderB : public ManagerCommon, public Ogre::SceneLoader {
 public:
  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) final;
  void load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node);

  std::shared_ptr<Ogre::TerrainGroup> GetTerrainGroup();
  void UpdatePbrParams();
  float GetHeigh(float x, float z);
  static void FixPbrParams(Ogre::MaterialPtr material);
  static void FixPbrShadowReceiver(Ogre::MaterialPtr material);
  static void FixPbrShadowCaster(Ogre::MaterialPtr material);
  static void FixPbrParams(const std::string &material);
  static void FixPbrShadowReceiver(const std::string &material);
  static void FixPbrShadowCaster(const std::string &material);
  static void UpdateForestParams(Ogre::MaterialPtr material);
  static void UpdateForestParams(const std::string &material);

 private:
  void ProcessScene(pugi::xml_node &xml_root);
  void ProcessNodes(pugi::xml_node &xml_node);
  void ProcessExternals(pugi::xml_node &xml_node);
  void ProcessEnvironment(pugi::xml_node &xml_node);
  void ProcessTerrainGroup(pugi::xml_node &xml_node);
  void ProcessTerrainLightmap(pugi::xml_node &xml_node);
  void ProcessUserData(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessCamera(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessNode(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessLookTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessTrackTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessEntity(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessParticleSystem(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessBillboardSet(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessPlane(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessForest(pugi::xml_node &xml_node);
  void ProcessFog(pugi::xml_node &xml_node);
  void ProcessSkyBox(pugi::xml_node &xml_node);
  void ProcessSkyDome(pugi::xml_node &xml_node);
  void ProcessSkyPlane(pugi::xml_node &xml_node);
  void ProcessLightRange(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation(pugi::xml_node &xml_node, Ogre::Light *light);

#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  void PageConstructed(size_t pagex, size_t pagez, float* heightData);
  void CreateTerrainHeightfieldShape(int size,
                                     float *data,
                                     const float &min_height,
                                     const float &max_height,
                                     const Ogre::Vector3 &position,
                                     const float &scale);
  void GetTerrainImage(bool flipX, bool flipY, Ogre::Image &ogre_image, const std::string &filename);
  void DefineTerrain(long x, long y, bool flat, const std::string &filename);
  void InitBlendMaps(Ogre::Terrain *terrain, int layer, const std::string &image);
#endif

  void EnsureHasTangents(Ogre::MeshPtr mesh_ptr);
  static bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration);

 private:
  Ogre::SceneManager *scene_manager_ = nullptr;
  Ogre::SceneNode *attach_node_ = nullptr;
  std::string group_name_;
  std::shared_ptr<Ogre::TerrainGroup> ogre_terrain_group_;
  std::shared_ptr<Ogre::TerrainPaging> terrain_paging_;
  std::vector<float> heigh_data_;
  Ogre::Vector4 terrain_box_;

  bool lod_generator_enable_ = false;
  bool physics_enable_ = true;

  bool terrain_fog_perpixel_ = true;
  bool terrain_receive_shadows_ = true;
  bool terrain_receive_shadows_low_lod_ = false;
  bool terrain_cast_shadows_ = false;
  bool terrain_lightmap_enable_ = true;
  bool terrain_colourmap_enable_ = false;
  std::string terrain_lightmap_;
  bool terrain_parallaxmap_enable_ = true;
  bool terrain_raybox_calculation_ = true;
  bool terrain_specularmap_enable_ = true;
  bool terrain_normalmap_enable_ = true;
  bool terrain_save_terrains_ = false;
  int terrain_lightmap_size_ = 2048;
  bool terrain_created_ = false;
  bool legacy_terrain_ = false;
  bool terrain_generator_b_ = false;

 public:
  void Setup() final;
  void Reset() final;
  void ResetGlobal() final;

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;

 private:
  static DotSceneLoaderB DotSceneLoaderSingleton;

 public:
  static DotSceneLoaderB &GetSingleton();
  static DotSceneLoaderB *GetSingletonPtr();

};

}

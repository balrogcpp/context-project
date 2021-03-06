//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "Component.h"
#include "Singleton.h"
#include "CameraMan.h"
#include "ReflectionCamera.h"
#include "CubeMapCamera.h"
#include "Input.h"
#include "Forest.h"
#include "Landscape.h"
#include <OgreSceneLoader.h>
#include <OgreVector4.h>
#include <vector>
#include <string>
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
}

class SinbadCharacterController;

namespace xio {
class CameraMan;
class JsonConfigurator;
class Configurator;
class Renderer;
class Physics;
class Sound;
class Overlay;

class DotSceneLoaderB final : public Component, public Singleton<DotSceneLoaderB>, public Ogre::SceneLoader {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Cleanup() final;
  void Pause() final {}
  void Resume() final {}
  void Update(float time) final;

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) final;
  float GetHeigh(float x, float z);

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
  void ProcessLandscape_(pugi::xml_node &xml_node);
  void ProcessFog_(pugi::xml_node &xml_node);
  void ProcessSkyBox_(pugi::xml_node &xml_node);
  void ProcessSkyDome_(pugi::xml_node &xml_node);
  void ProcessSkyPlane_(pugi::xml_node &xml_node);
  void ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light);

  std::unique_ptr<ReflectionCamera> rcamera_;
  std::unique_ptr<CubeMapCamera> ccamera_;
  static inline std::unique_ptr<Landscape> terrain_;
  static inline std::unique_ptr<Forest> forest_;
  std::unique_ptr<CameraMan> camera_;
  view_ptr<Ogre::SceneManager> scene_;
  view_ptr<Ogre::Root> root_;
  view_ptr<Ogre::SceneNode> root_node_;
  view_ptr<Ogre::SceneNode> attach_node_;
  std::string group_name_;
  std::unique_ptr<SinbadCharacterController> sinbad_;

  static inline view_ptr<Configurator> conf_;
  static inline view_ptr<Renderer> renderer_;
  static inline view_ptr<Physics> physics_;
  static inline view_ptr<Sound> sound_;
  static inline view_ptr<Overlay> overlay_;
  static inline view_ptr<InputSequencer> input_;
  
 public:
//----------------------------------------------------------------------------------------------------------------------
  static void LocateComponents(view_ptr<Configurator> conf,
							   view_ptr<InputSequencer> input,
							   view_ptr<Renderer> renderer,
							   view_ptr<Physics> physics,
							   view_ptr<Sound> sounds,
							   view_ptr<Overlay> overlay) {
	conf_ = conf;
	input_ = input;
	renderer_ = renderer;
	physics_ = physics;
	sound_ = sounds;
	overlay_ = overlay;
  }
//----------------------------------------------------------------------------------------------------------------------
  static Landscape &GetTerrain() {
    return *terrain_;
  }
//----------------------------------------------------------------------------------------------------------------------
  CameraMan &GetCamera() const {
    return *camera_;
  }
//----------------------------------------------------------------------------------------------------------------------
  static Forest &GetForest() {
    return *forest_;
  }
};
}
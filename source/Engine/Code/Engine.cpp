/// created by Andrey Vasiliev

#include "pch.h"
#include "Engine.h"
#include "Observer.h"
#include "SinbadCharacterController.h"
extern "C" {
#include <SDL2/SDL_messagebox.h>
}
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

int ErrorWindow(const char *caption, const char *text) {
#ifdef _WIN32
  MessageBox(nullptr, text, caption, MB_ICONERROR);
#else
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption, text, nullptr);
#endif
#ifdef EMSCRIPTEN
  emscripten_pause_main_loop();
#endif
  return -1;
}

namespace Glue {

Engine &GetEngine() {
  static auto &EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

PhysicsManager &GetPhysics() { return GetComponent<PhysicsManager>(); }

AudioManager &GetAudio() { return GetComponent<AudioManager>(); }

Engine::Engine() {}

Engine::~Engine() {}

void Engine::Init() {
  video = make_unique<VideoManager>();
  RegComponent(video.get());

  compositor = make_unique<CompositorManager>();
  RegComponent(compositor.get());

  physics = make_unique<PhysicsManager>();
  RegComponent(physics.get());

  audio = make_unique<AudioManager>();
  RegComponent(audio.get());

  //  AddFont("NotoSans-Regular", RGN_INTERNAL, nullptr, io.Fonts->GetGlyphRangesCyrillic());
  //  ImFontConfig config;
  //  config.MergeMode = true;
  //  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  //  AddFont("KenneyIcon-Regular", RGN_INTERNAL, &config, icon_ranges);

  //  auto *TGO = TerrainGlobalOptions::getSingletonPtr();
  //  if (!TGO) TGO = new TerrainGlobalOptions();
  //  TGO->setUseVertexCompressionWhenAvailable(true);
  //  TGO->setCastsDynamicShadows(false);
  //  TGO->setCompositeMapDistance(300);
  //  TGO->setMaxPixelError(8);
  //  TGO->setUseRayBoxDistanceCalculation(true);
  //  TGO->setDefaultMaterialGenerator(make_shared<TerrainMaterialGeneratorB>());
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

// constexpr ResolutionItem ResolutionList[] = {
//    {"360x800", 360, 800},
//    {"800x600", 800, 600},
//    {"810x1080", 810, 1080},
//    {"768x1024", 768, 1024},
//    {"834x1112", 834, 1112},
//    {"1024x768", 1024, 768},
//    {"1024x1366", 1024, 1366},
//    {"1280x720", 1280, 720},
//    {"1280x800", 1280, 800},
//    {"1280x1024", 1280, 1024},
//    {"1360x768", 1360, 768},
//    {"1366x768", 1366, 768},
//    {"1440x900", 1440, 900},
//    {"1536x864", 1536, 864},
//    {"1600x900", 1600, 900},
//    {"1680x1050", 1680, 1050},
//    {"1920x1080", 1920, 1080},
//    {"1920x1200", 1920, 1200},
//    {"2048x1152", 2048, 1152},
//    {"2560x1440", 2560, 1440},
//};

static bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
    const auto &vertexElem = *iter;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES) hasUVs = true;

    ++iter;
  }

  return !hasTangents && hasUVs;
}

static void EnsureHasTangents(const Ogre::MeshPtr &MeshSPtr) {
  bool generateTangents = false;
  if (MeshSPtr->sharedVertexData) {
    auto *vertexDecl = MeshSPtr->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < MeshSPtr->getNumSubMeshes(); ++i) {
    auto *subMesh = MeshSPtr->getSubMesh(i);
    if (subMesh->vertexData) {
      auto *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    MeshSPtr->buildTangentVectors();
  }
}

static void FixTransparentShadowCaster(const Ogre::MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  string MaterialName = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    string caster_name = "PSSM/shadow_caster_alpha/" + MaterialName;
    auto new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureName(texture_name);
        }
      }
    }

    auto *new_pass = new_caster->getTechnique(0)->getPass(0);
    new_pass->setCullingMode(pass->getCullingMode());
    new_pass->setManualCullingMode(pass->getManualCullingMode());
    new_pass->setAlphaRejectValue(alpha_rejection);

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

static void FixTransparentShadowCaster(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  FixTransparentShadowCaster(material_ptr);
}

static void AddMaterial(const Ogre::MaterialPtr &material) {
  FixTransparentShadowCaster(material);
  GetEngine().AddMaterial(material);
}

static void AddMaterial(const string &material) {
  auto MaterialSPtr = Ogre::MaterialManager::getSingleton().getByName(material);
  if (MaterialSPtr) AddMaterial(MaterialSPtr);
}

static void AddMeshMaterial(Ogre::MeshPtr MeshSPtr, const string &MaterialName) {
  try {
    EnsureHasTangents(MeshSPtr);

    for (auto &submesh : MeshSPtr->getSubMeshes()) {
      Ogre::MaterialPtr material;

      if (!MaterialName.empty()) {
        submesh->setMaterialName(MaterialName);
      }

      material = submesh->getMaterial();
      if (material) AddMaterial(material);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

static void AddMeshMaterial(const string &MeshName, const string &MaterialName = "") {
  const auto &MeshSPtr = Ogre::MeshManager::getSingleton().getByName(MeshName);
  AddMeshMaterial(MeshSPtr, MaterialName);
}

static void AddEntityMaterial(Ogre::Entity *EntityPtr, const string &MaterialName = "") {
  try {
    if (!MaterialName.empty()) {
      auto material = Ogre::MaterialManager::getSingleton().getByName(MaterialName);
      if (material) {
        AddMaterial(material);
        EntityPtr->setMaterial(material);
      }
    }

    AddMeshMaterial(EntityPtr->getMesh(), MaterialName);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

float Engine::GetHeight(float x, float z) {
  //  if (terrainGroup)
  //    return terrainGroup->getHeightAtWorldPosition(x, 1000, z);
  //  else
  //    return 0.0f;
  return 0.0;
}

void Engine::AddEntity(Ogre::Entity *EntityPtr) { AddEntityMaterial(EntityPtr); }

void Engine::AddMaterial(Ogre::MaterialPtr material) {
  //  const auto *Pass = material->getTechnique(0)->getPass(0);
  //  if (Pass->hasVertexProgram()) gpuVpParams.push_back(Pass->getVertexProgramParameters());
  //  if (Pass->hasFragmentProgram()) gpuFpParams.push_back(Pass->getFragmentProgramParameters());
}

void Engine::AddMaterial(const string &MaterialName) {
  //  auto MaterialPtr = MaterialManager::getSingleton().getByName(MaterialName);
  //  if (MaterialPtr) AddMaterial(MaterialPtr);
}

void Engine::AddCamera(Ogre::Camera *OgreCameraPtr) {}

void Engine::AddSinbad(Ogre::Camera *OgreCameraPtr) {
  sinbad = make_unique<SinbadCharacterController>(OgreCameraPtr);
  InputSequencer::GetInstance().RegObserver(sinbad.get());
}

// void Engine::AddForests(Forests::PagedGeometry *PGPtr, const string &MaterialName) {
//  pgList.push_back(unique_ptr<Forests::PagedGeometry>(PGPtr));
//  if (!MaterialName.empty()) AddMaterial(MaterialName);
//}
//
// void Engine::AddTerrain(TerrainGroup *TGP) { terrainGroup.reset(TGP); }

void Engine::RegComponent(SystemI *component) {
  component->OnSetUp();
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it == componentList.end()) componentList.push_back(component);
}

void Engine::UnRegComponent(SystemI *component) {
  component->OnClean();
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it != componentList.end()) componentList.erase(it);
}

void Engine::OnPause() {
  for (auto &it : componentList) it->OnPause();
}

void Engine::OnResume() {
  for (auto &it : componentList) it->OnResume();
}

void Engine::OnMenuOn() { physics->OnPause(); }

void Engine::OnMenuOff() { physics->OnResume(); }

void Engine::OnCleanup() {
  for (auto &it : componentList) it->OnClean();
  InputSequencer::GetInstance().UnRegObserver(sinbad.get());
  sinbad.reset();
  //  pgList.clear();
  //  if (terrainGroup) terrainGroup->removeAllTerrains();
  //  terrainGroup.reset();
  //  delete TerrainGlobalOptions::getSingletonPtr();
}

void Engine::Update(float PassedTime) {
  if (paused) return;
  if (sinbad) sinbad->Update(PassedTime);
  //  for (auto &it : pgList) it->update();
  for (auto &it : componentList) it->OnUpdate(PassedTime);

  static ImGuiIO &io = ImGui::GetIO();
  Ogre::ImGuiOverlay::NewFrame();
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void Engine::RenderFrame() { video->RenderFrame(); }

}  // namespace Glue

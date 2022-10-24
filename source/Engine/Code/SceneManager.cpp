/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "SkyModel/ArHosekSkyModel.h"
#include "SkyModel/SkyModel.h"

using namespace std;

namespace {

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
  Glue::GetComponent<Glue::SceneManager>().RegMaterial(material);
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

}  // namespace

namespace Glue {

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::OnSetUp() {
  sceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  camera = sceneManager->getCamera("Default");
}

void SceneManager::OnClean() {
  InputSequencer::GetInstance().UnRegObserver(sinbad.get());
  sinbad.reset();
  skyBoxFpParams.reset();
  gpuFpParams.clear();
  gpuFpParams.shrink_to_fit();
  gpuVpParams.clear();
  gpuVpParams.shrink_to_fit();
}

void SceneManager::OnPause() {}

void SceneManager::OnResume() {}

void SceneManager::OnUpdate(float time) {
  if (sinbad) sinbad->Update(time);
  static Ogre::Matrix4 MVP;
  static Ogre::Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix();
  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void SceneManager::LoadFromFile(const std::string filename) {
  auto *rootNode = sceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);
}

Ogre::Vector3 SceneManager::GetSunPosition() {
  auto *SunPtr = sceneManager->getLight("Sun");
  return SunPtr ? -SunPtr->getDerivedDirection().normalisedCopy() : Ogre::Vector3::ZERO;
}

void SceneManager::SetUpSky() {
  auto colorspace = ColorSpace::sRGB;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Ogre::Vector3(1.0);
  auto sunColor = Ogre::Vector3(20000);
  auto sunDir = GetSunPosition();

  SkyModel sky;
  sky.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {sky.StateX, sky.StateY, sky.StateZ};

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = States[j]->configs[j][i];
  hosekParams[9] = Ogre::Vector3(sky.StateX->radiances[0], sky.StateY->radiances[1], sky.StateZ->radiances[2]);

  auto SkyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) skyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < hosekParamList.size(); i++) FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void SceneManager::RegCamera(Ogre::Camera *camera) {}

void SceneManager::RegSinbad(Ogre::Camera *camera) {
  sinbad = make_unique<SinbadCharacterController>(camera);
  InputSequencer::GetInstance().RegObserver(sinbad.get());
}

void SceneManager::RegEntity(Ogre::Entity *entity) { AddEntityMaterial(entity); }

void SceneManager::RegMaterial(const Ogre::MaterialPtr &material) {
  const auto *pass = material->getTechnique(0)->getPass(0);
  if (pass->hasVertexProgram()) gpuVpParams.push_back(pass->getVertexProgramParameters());
  if (pass->hasFragmentProgram()) gpuFpParams.push_back(pass->getFragmentProgramParameters());
}

void SceneManager::RegMaterial(const std::string &name) {
  auto material = Ogre::MaterialManager::getSingleton().getByName(name);
  if (material) AddMaterial(material);
}

}  // namespace Glue

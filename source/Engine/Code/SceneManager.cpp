/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "SkyModel/ArHosekSkyModel.h"
#include "SkyModel/SkyModel.h"

using namespace std;

namespace {

void CheckTransparentShadowCaster(const Ogre::MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  std::string MaterialName = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    std::string caster_name = "PSSM/shadow_caster_alpha/" + MaterialName;
    auto new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

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

void CheckTransparentShadowCaster(const std::string &material) {
  auto ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  CheckTransparentShadowCaster(ptr);
}

bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto it = elementList.begin();
  auto end = elementList.end();

  while (it != end && !hasTangents) {
    const auto &vertexElem = *it;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES) hasUVs = true;

    ++it;
  }

  return !hasTangents && hasUVs;
}

void EnsureHasTangents(const Ogre::MeshPtr &mesh) {
  bool generateTangents = false;

  if (mesh->sharedVertexData) {
    auto *vertexDecl = mesh->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (int i = 0; i < mesh->getNumSubMeshes(); ++i) {
    auto *subMesh = mesh->getSubMesh(i);
    if (subMesh->vertexData) {
      auto *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    mesh->buildTangentVectors();
  }
}

}  // namespace

namespace Glue {

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::OnSetUp() {
  sceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  ogreCamera = sceneManager->getCamera("Default");
}

void SceneManager::OnClean() {
  InputSequencer::GetInstance().UnRegObserver(sinbad.get());
  sinbad.reset();
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
  MVP = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void SceneManager::LoadFromFile(const std::string filename) {
  auto *rootNode = sceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);

  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }

  ScanNode(rootNode);
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

  SkyModel skyModel;
  skyModel.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {skyModel.StateX, skyModel.StateY, skyModel.StateZ};

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 3; j++) {
      hosekParams[i][j] = States[j]->configs[j][i];
      hosekParams[9] = Ogre::Vector3(skyModel.StateX->radiances[0], skyModel.StateY->radiances[1], skyModel.StateZ->radiances[2]);
    }
  }
  auto skyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  auto FpParams = skyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) skyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < hosekParamList.size(); i++) FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void SceneManager::RegCamera(Ogre::Camera *camera) {
  if (!sinbad && camera->getName() == "Default") {
    sinbad = make_unique<SinbadCharacterController>(camera);
    InputSequencer::GetInstance().RegObserver(sinbad.get());
  }
  //printf("[ScanNode] RegCamera %s\n", camera->getName().c_str());
}

void SceneManager::RegLight(Ogre::Light *light) {
  //printf("[ScanNode] RegLight %s\n", light->getName().c_str());
}

void SceneManager::RegEntity(Ogre::Entity *entity) {
  //printf("[ScanNode] RegEntity %s\n", entity->getName().c_str());

  EnsureHasTangents(entity->getMesh());

  if (entity->hasSkeleton()) {
    for (auto it : entity->getAttachedObjects()) {
      if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
        RegCamera(camera);
        continue;
      }

      if (auto light = dynamic_cast<Ogre::Light *>(it)) {
        RegLight(light);
        continue;
      }

      if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
        RegEntity(entity);
        continue;
      }
    }
  }

  for (const auto &it : entity->getSubEntities()) {
    RegMaterial(it->getMaterial());
  }

  auto objBindings = entity->getUserObjectBindings();
  if (objBindings.getUserAny("proxy").has_value()) {
    Glue::GetComponent<Glue::PhysicsManager>().ProcessData(entity, objBindings);
  }
}

void SceneManager::RegMaterial(const Ogre::MaterialPtr &material) {
  const auto *pass = material->getTechnique(0)->getPass(0);
  CheckTransparentShadowCaster(material);

  if (pass->hasVertexProgram()) {
    const auto ptr = pass->getVertexProgramParameters();
    if (find(gpuVpParams.begin(), gpuVpParams.end(), ptr) == gpuVpParams.end()) gpuVpParams.push_back(ptr);
  }

  if (pass->hasFragmentProgram()) {
    const auto ptr = pass->getFragmentProgramParameters();
    if (find(gpuFpParams.begin(), gpuFpParams.end(), ptr) == gpuFpParams.end()) gpuFpParams.push_back(ptr);
  }
}

void SceneManager::RegMaterial(const std::string &name) {
  auto material = Ogre::MaterialManager::getSingleton().getByName(name);
  RegMaterial(material);
}

void SceneManager::ScanNode(Ogre::SceneNode *node) {
  for (auto it : node->getAttachedObjects()) {
    if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
      RegCamera(camera);
      continue;
    }

    if (auto light = dynamic_cast<Ogre::Light *>(it)) {
      RegLight(light);
      continue;
    }

    if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
      RegEntity(entity);
      continue;
    }

    Ogre::LogManager::getSingleton().logWarning("DotSceneLoaderB - unsupported MovableType " + it->getMovableType());
  }

  // recurse
  for (auto it : node->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

}  // namespace Glue

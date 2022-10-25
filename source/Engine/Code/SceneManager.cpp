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

  if (pass->getNumTextureUnitStates() > 0 && pass->getAlphaRejectValue() > 0 && material->getTransparencyCastsShadows()) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    std::string caster_name = "PSSM/shadow_caster_alpha/" + material->getName();
    auto newCaster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!newCaster) {
      newCaster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = newCaster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureName(texture_name);
        }
      }
    }

    auto *newPass = newCaster->getTechnique(0)->getPass(0);
    newPass->setCullingMode(pass->getCullingMode());
    newPass->setManualCullingMode(pass->getManualCullingMode());
    newPass->setAlphaRejectValue(pass->getAlphaRejectValue());
    material->getTechnique(0)->setShadowCasterMaterial(newCaster);
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
  ogreRoot = Ogre::Root::getSingletonPtr();
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
  for (int i = 0; i < hosekParamList.size(); i++) {
    FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
  }
}

void SceneManager::LoadFromFile(const std::string filename) {
  auto *rootNode = sceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);

  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }

  if (!sinbad && sceneManager->hasCamera("Default")) {
    sinbad = make_unique<SinbadCharacterController>(sceneManager->getCamera("Default"));
    InputSequencer::GetInstance().RegObserver(sinbad.get());
  }

  // scan second time, new objects added during first scan
  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::RegCamera(Ogre::Camera *camera) {}

void SceneManager::RegLight(Ogre::Light *light) {}

void SceneManager::RegEntity(Ogre::Entity *entity) {
  EnsureHasTangents(entity->getMesh());

  if (entity->hasSkeleton()) {
    for (auto it : entity->getAttachedObjects()) {
      if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Camera: " + it->getName());
        RegCamera(camera);
        continue;
      }

      if (auto light = dynamic_cast<Ogre::Light *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Light: " + it->getName());
        ;
        RegLight(light);
        continue;
      }

      if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Entity: " + it->getName());
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
  CheckTransparentShadowCaster(material);

  const auto *pass = material->getTechnique(0)->getPass(0);

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
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Camera: " + it->getName());
      RegCamera(camera);
      continue;
    }

    if (auto light = dynamic_cast<Ogre::Light *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Light: " + it->getName());
      RegLight(light);
      continue;
    }

    if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Entity: " + it->getName());
      RegEntity(entity);
      continue;
    }
  }

  // recurse
  for (auto it : node->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

}  // namespace Glue

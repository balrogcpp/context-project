// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "ShaderHelpers.h"
#include "Engine.h"
#include "Log.h"

using namespace std;
using namespace Ogre;

namespace Glue {

static bool HasNoTangentsAndCanGenerate(VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
    const VertexElement &vertexElem = *iter;
    if (vertexElem.getSemantic() == VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == VES_TEXTURE_COORDINATES) hasUVs = true;

    ++iter;
  }

  return !hasTangents && hasUVs;
}

void EnsureHasTangents(MeshPtr mesh) {
  bool generateTangents = false;
  if (mesh->sharedVertexData) {
    VertexDeclaration *vertexDecl = mesh->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < mesh->getNumSubMeshes(); ++i) {
    SubMesh *subMesh = mesh->getSubMesh(i);
    if (subMesh->vertexData) {
      VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    mesh->buildTangentVectors();
  }
}

void FixTransparentShadowCaster(const MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  string MaterialName = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    string caster_name = "PSSM/shadow_caster_alpha/" + MaterialName;
    MaterialPtr new_caster = MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(TextureUnitState::CONTENT_NAMED);
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

void FixTransparentShadowCaster(const string &material) {
  auto material_ptr = MaterialManager::getSingleton().getByName(material);
  FixTransparentShadowCaster(material_ptr);
}

static void FixMaterial(const MaterialPtr &material) {
  FixTransparentShadowCaster(material);
  GetScene().AddMaterial(material);
}

void FixMeshMaterial(MeshPtr mesh, const string &MaterialName) {
  try {
    //EnsureHasTangents(mesh);

    for (auto &submesh : mesh->getSubMeshes()) {
      MaterialPtr material;

      if (!MaterialName.empty()) {
        submesh->setMaterialName(MaterialName);
      }

      material = submesh->getMaterial();
      if (material) FixMaterial(material);
    }
  } catch (Ogre::Exception &e) {
    Log::Message("[DotSceneLoader] Error loading an entity!");
    Log::Message(e.getFullDescription());
  }
}

void FixMeshMaterial(const string &MeshName, const string &MaterialName) {
  const auto &mesh = MeshManager::getSingleton().getByName(MeshName);
  FixMeshMaterial(mesh, MaterialName);
}

void FixEntityMaterial(Entity *entity, const string &MaterialName) {
  try {
    if (!MaterialName.empty()) {
      auto material = MaterialManager::getSingleton().getByName(MaterialName);
      if (material) {
        FixMaterial(material);
        entity->setMaterial(material);
      }
    }

    FixMeshMaterial(entity->getMesh(), MaterialName);
  } catch (Ogre::Exception &e) {
    LogManager::getSingleton().logMessage(e.getFullDescription());
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

}  // namespace Glue
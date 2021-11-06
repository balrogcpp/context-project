// This source file is part of context-project
// Created by Andrew Vasiliev

#include "MeshUtils.h"

#include "PbrShaderUtils.h"
#include "pcheader.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
static bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
    const Ogre::VertexElement &vertexElem = *iter;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES) hasUVs = true;

    ++iter;
  }

  return !hasTangents && hasUVs;
}

//----------------------------------------------------------------------------------------------------------------------
void EnsureHasTangents(Ogre::MeshPtr mesh) {
  bool generateTangents = false;
  if (mesh->sharedVertexData) {
    Ogre::VertexDeclaration *vertexDecl = mesh->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < mesh->getNumSubMeshes(); ++i) {
    Ogre::SubMesh *subMesh = mesh->getSubMesh(i);
    if (subMesh->vertexData) {
      Ogre::VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    mesh->buildTangentVectors();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateMeshMaterial(Ogre::MeshPtr mesh, bool cast_shadows, const string &material_name, bool planar_reflection,
                        bool active_ibl) {
  try {
    EnsureHasTangents(mesh);

    for (auto &submesh : mesh->getSubMeshes()) {
      Ogre::MaterialPtr material;

      if (!material_name.empty()) {
        submesh->setMaterialName(material_name);
      }

      material = submesh->getMaterial();

      if (material) {
        Pbr::UpdatePbrParams(material);

        if (cast_shadows) Pbr::UpdatePbrShadowCaster(material);

        if (material->getReceiveShadows()) Pbr::UpdatePbrShadowReceiver(material);

        Pbr::UpdatePbrIbl(material, active_ibl);
      }
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateMeshMaterial(const string &mesh_name, bool cast_shadows, const string &material_name, bool planar_reflection,
                        bool active_ibl) {
  const auto &mesh = Ogre::MeshManager::getSingleton().getByName(mesh_name);
  UpdateMeshMaterial(mesh, cast_shadows, material_name, planar_reflection, active_ibl);
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateEntityMaterial(Ogre::Entity *entity, bool cast_shadows, const string &material_name, bool planar_reflection,
                          bool active_ibl) {
  try {
    entity->setCastShadows(cast_shadows);

    if (!material_name.empty()) {
      entity->setMaterialName(material_name);

      Ogre::MaterialPtr material;
      material = Ogre::MaterialManager::getSingleton().getByName(material_name);

      if (material) {
        Pbr::UpdatePbrParams(material);

        if (cast_shadows) Pbr::UpdatePbrShadowCaster(material);

        if (material->getReceiveShadows()) Pbr::UpdatePbrShadowReceiver(material);

        Pbr::UpdatePbrIbl(material, active_ibl);
      }
    }

    UpdateMeshMaterial(entity->getMesh(), cast_shadows, material_name, planar_reflection, active_ibl);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

}  // namespace glue

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

#include "MeshUtils.h"
#include "PbrShaderUtils.h"

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
	const Ogre::VertexElement &vertexElem = *iter;
	if (vertexElem.getSemantic() == Ogre::VES_TANGENT)
	  hasTangents = true;
	if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
	  hasUVs = true;

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
void UpdateMeshEdgeList(Ogre::Entity *entity) {
  if (!entity->getMesh()->isEdgeListBuilt()) {
	entity->getMesh()->buildEdgeList();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateMeshMaterial(Ogre::MeshPtr mesh, bool cast_shadows, const std::string &material_name, bool planar_reflection, bool active_ibl) {
  try {
	EnsureHasTangents(mesh);

	for (auto &submesh : mesh->getSubMeshes()) {
	  Ogre::MaterialPtr material;

	  if (!material_name.empty()) {
		submesh->setMaterialName(material_name);
	  }

	  material = submesh->getMaterial();

	  if (material) {
		UpdatePbrParams(material);

		if (cast_shadows)
		  UpdatePbrShadowCaster(material);

		if (material->getReceiveShadows())
		  UpdatePbrShadowReceiver(material);

		UpdatePbrIbl(material, active_ibl);
	  }
	}
  }
  catch (Ogre::Exception &e) {
	Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
	Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateMeshMaterial(const std::string &mesh_name, bool cast_shadows, const std::string &material_name, bool planar_reflection, bool active_ibl) {
  const auto &mesh = Ogre::MeshManager::getSingleton().getByName(mesh_name);
  UpdateMeshMaterial(mesh, cast_shadows, material_name, planar_reflection, active_ibl);
}

//----------------------------------------------------------------------------------------------------------------------
void UpdateEntityMaterial(Ogre::Entity *entity, bool cast_shadows, const std::string &material_name, bool planar_reflection, bool active_ibl) {
  try {
	entity->setCastShadows(cast_shadows);

	UpdateMeshMaterial(entity->getMesh(), cast_shadows, material_name, planar_reflection, active_ibl);
  }
  catch (Ogre::Exception &e) {
	Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
	Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

} //namespace
// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <vector>

namespace Ogre {
class Entity;
}

namespace Glue {

///
void FixTransparentShadowCaster(const Ogre::MaterialPtr &material);

///
void FixTransparentShadowCaster(const std::string &material);

///
void EnsureHasTangents(Ogre::MeshPtr MeshSPtr);

///
void AddMaterial(const Ogre::MaterialPtr &material);

///
void AddMaterial(const std::string &material);

///
void AddMeshMaterial(Ogre::MeshPtr MeshSPtr, const std::string &MaterialName = "");

///
void AddMeshMaterial(const std::string &MeshName, const std::string &MaterialName = "");

///
void AddEntityMaterial(Ogre::Entity *EntityPtr, const std::string &MaterialName = "");

}  // namespace Glue

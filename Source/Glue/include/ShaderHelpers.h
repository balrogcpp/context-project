// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <vector>

namespace Ogre {
class Camera;
}

namespace Glue {

///
void FixTransparentShadowCaster(const Ogre::MaterialPtr &material);

///
void FixTransparentShadowCaster(const std::string &material);

///
void EnsureHasTangents(Ogre::MeshPtr mesh);

///
void FixMeshMaterial(Ogre::MeshPtr mesh, const std::string &MaterialName);

///
void FixMeshMaterial(const std::string &MeshName, const std::string &MaterialName);

///
void FixEntityMaterial(Ogre::Entity *entity, const std::string &MaterialName);

}  // namespace Glue

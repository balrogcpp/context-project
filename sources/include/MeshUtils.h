// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once

#include <OgreMesh.h>

#include <string>

namespace Ogre {
class VertexDeclaration;
class Entity;
}  // namespace Ogre

namespace xio {

void EnsureHasTangents(Ogre::MeshPtr mesh);

void UpdateMeshMaterial(Ogre::MeshPtr mesh, bool cast_shadows = true, const std::string &material_name = "",
                        bool planar_reflection = false, bool active_ibl = false);

void UpdateMeshMaterial(const std::string &mesh_name, bool cast_shadows = true, const std::string &material_name = "",
                        bool planar_reflection = false, bool active_ibl = false);

void UpdateEntityMaterial(Ogre::Entity *entity, bool cast_shadows = true, const std::string &material_name = "",
                          bool planar_reflection = false, bool active_ibl = false);

}  // namespace xio

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

#pragma once

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreVector.h>
#include <string>
#include <vector>
#include <memory>

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameterAuto(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                     const std::string &parameter_name,
                                     const Ogre::GpuProgramParameters::AutoConstantType value_name,
                                     const int info = 0) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedAutoConstant(parameter_name, value_name, info);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Vector4 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Vector3 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                          const std::string &parameter_name,
                          Ogre::Vector2 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 float value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 int value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 unsigned int value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}

void UpdatePbrParams(Ogre::MaterialPtr material);
void UpdatePbrParams(const std::string &material);

void UpdatePbrShadowReceiver(Ogre::MaterialPtr material);
void UpdatePbrShadowReceiver(const std::string &material);

void UpdatePbrShadowCaster(Ogre::MaterialPtr material);
void UpdatePbrShadowCaster(const std::string &material);

void EnsureHasTangents(Ogre::MeshPtr mesh_ptr);
bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration);
}
// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include <OgreColourValue.h>
#include <OgreQuaternion.h>
#include <OgreVector.h>
#include <string>

namespace pugi {
class xml_node;
}

namespace Ogre {
class SceneManager;
class SceneNode;
class TerrainGroup;
class TerrainGlobalOptions;
class VertexDeclaration;
}  // namespace Ogre

namespace Glue {

template <typename T>
std::string ToString(T t) {
  return t == 0 ? std::string() : std::to_string(t);
}

template <typename T>
const char *ToCString(T t) {
  return t == 0 ? "" : std::to_string(t).c_str();
}

inline float ToFloat(const std::string &s, float defaultValue = 0) {
  return s.empty() ? defaultValue : std::atof(s.c_str());
}
inline float ToInt(const std::string &s, int defaultValue = 0) {
  return s.empty() ? defaultValue : std::atoi(s.c_str());
}

std::string GetAttrib(const pugi::xml_node &XmlNode, const std::string &attrib, const std::string &defaultValue = "");
float GetAttribReal(const pugi::xml_node &XmlNode, const std::string &attrib, float defaultValue = 0);
int GetAttribInt(const pugi::xml_node &XmlNode, const std::string &attrib, int defaultValue = 0);
bool GetAttribBool(const pugi::xml_node &XmlNode, const std::string &attrib, bool defaultValue = false);
Ogre::Vector3 ParseVector3(const pugi::xml_node &XmlNode);
Ogre::Vector3 ParsePosition(const pugi::xml_node &XmlNode);
Ogre::Vector3 ParseScale(const pugi::xml_node &XmlNode);
Ogre::Quaternion ParseRotation(const pugi::xml_node &XmlNode);
Ogre::Quaternion ParseQuaternion(const pugi::xml_node& XmlNode);
Ogre::ColourValue ParseColour(pugi::xml_node &XmlNode);

}  // namespace Glue

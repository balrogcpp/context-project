//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "XmlUtils.h"

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
std::string GetAttrib(const pugi::xml_node &xml_node,
					  const std::string &attrib,
					  const std::string &defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
	return anode.value();
  } else {
	return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib, float defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
	return Ogre::StringConverter::parseReal(anode.value());
  } else {
	return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib, int defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
	return Ogre::StringConverter::parseInt(anode.value());
  } else {
	return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib, bool defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
	return anode.as_bool();
  } else {
	return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
void ParseMaterial(Ogre::Entity *ent, const std::string &material_name) {
  Ogre::MaterialPtr material;
  if (!material_name.empty()) {
	material = Ogre::MaterialManager::getSingleton().getByName(material_name);

  }

  for (size_t i; i < ent->getNumManualLodLevels(); i++) {
	auto *lod = ent->getManualLodLevel(i);

	for (size_t j; j < lod->getNumSubEntities(); j++) {
	  auto *subEnt = lod->getSubEntity(j);
	  if (!material) {
		material = ent->getManualLodLevel(1)->getSubEntity(j)->getMaterial();
	  }
	  subEnt->setMaterial(material);
	}
  }
//  }
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node) {
  Ogre::Quaternion orientation;

  if (xml_node.attribute("qw")) {
	orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("qw").value());
	orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("qx").value());
	orientation.y = -Ogre::StringConverter::parseReal(xml_node.attribute("qz").value());
	orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("qy").value());
  } else if (xml_node.attribute("axisX")) {
	Ogre::Vector3 axis;
	axis.x = Ogre::StringConverter::parseReal(xml_node.attribute("axisX").value());
	axis.y = Ogre::StringConverter::parseReal(xml_node.attribute("axisY").value());
	axis.z = Ogre::StringConverter::parseReal(xml_node.attribute("axisZ").value());
	float angle = Ogre::StringConverter::parseReal(xml_node.attribute("angle").value());

	orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (xml_node.attribute("angleX")) {
	Ogre::Matrix3 rot;
	rot.FromEulerAnglesXYZ(Ogre::StringConverter::parseAngle(xml_node.attribute("angleX").value()),
						   Ogre::StringConverter::parseAngle(xml_node.attribute("angleY").value()),
						   Ogre::StringConverter::parseAngle(xml_node.attribute("angleZ").value()));
	orientation.FromRotationMatrix(rot);
  } else if (xml_node.attribute("x")) {
	orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
	orientation.y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
	orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());
	orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("w").value());
  } else if (xml_node.attribute("w")) {
	orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("w").value());
	orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
	orientation.y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
	orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());
  }

  Ogre::Vector3 direction = orientation*Ogre::Vector3::NEGATIVE_UNIT_Z;
  float x = direction.x;
  float y = direction.y;
  float z = direction.z;
  direction = Ogre::Vector3(x, z, -y).normalisedCopy();

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *node = scene->getRootSceneNode()->createChildSceneNode("tmp");
  node->setDirection(direction);
  orientation = node->getOrientation();
  scene->destroySceneNode(node);

  return orientation;
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::ColourValue ParseColour(pugi::xml_node &xml_node) {
  return Ogre::ColourValue(Ogre::StringConverter::parseReal(xml_node.attribute("r").value()),
						   Ogre::StringConverter::parseReal(xml_node.attribute("g").value()),
						   Ogre::StringConverter::parseReal(xml_node.attribute("b").value()),
						   xml_node.attribute("a")
						   ? Ogre::StringConverter::parseReal(xml_node.attribute("a").value()) : 1);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::ColourValue ParseProperty(pugi::xml_node &xml_node) {
  std::string name = GetAttrib(xml_node, "name");
  std::string type = GetAttrib(xml_node, "type");
  std::string data = GetAttrib(xml_node, "data");

  return Ogre::ColourValue(Ogre::StringConverter::parseReal(xml_node.attribute("r").value()),
						   Ogre::StringConverter::parseReal(xml_node.attribute("g").value()),
						   Ogre::StringConverter::parseReal(xml_node.attribute("b").value()),
						   xml_node.attribute("a")
						   ? Ogre::StringConverter::parseReal(xml_node.attribute("a").value()) : 1);
}
}
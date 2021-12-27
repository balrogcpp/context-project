// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Components/XmlParser.h"
#include <pugixml.hpp>

using namespace std;

namespace Glue {

string GetAttrib(const pugi::xml_node &XmlNode, const string &attrib, const string &defaultValue) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return anode.value();
  } else {
    return defaultValue;
  }
}

float GetAttribReal(const pugi::xml_node &XmlNode, const string &attrib, float defaultValue) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return ToFloat(anode.value());
  } else {
    return defaultValue;
  }
}

int GetAttribInt(const pugi::xml_node &XmlNode, const string &attrib, int defaultValue) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return ToInt(anode.value());
  } else {
    return defaultValue;
  }
}

bool GetAttribBool(const pugi::xml_node &XmlNode, const string &attrib, bool defaultValue) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return anode.as_bool();
  } else {
    return defaultValue;
  }
}

Ogre::Vector3 ParseVector3(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Vector3 ParsePosition(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Vector3 ParseScale(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Quaternion ParseRotation(const pugi::xml_node &XmlNode) {
  Ogre::Quaternion orientation;

  if (XmlNode.attribute("qw")) {
    orientation.w = ToFloat(XmlNode.attribute("qw").value());
    orientation.x = ToFloat(XmlNode.attribute("qx").value());
    orientation.y = -ToFloat(XmlNode.attribute("qz").value());
    orientation.z = ToFloat(XmlNode.attribute("qy").value());
  } else if (XmlNode.attribute("axisX")) {
    Ogre::Vector3 axis;
    axis.x = ToFloat(XmlNode.attribute("axisX").value());
    axis.y = ToFloat(XmlNode.attribute("axisY").value());
    axis.z = ToFloat(XmlNode.attribute("axisZ").value());
    float angle = ToFloat(XmlNode.attribute("angle").value());

    orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (XmlNode.attribute("angleX")) {
    Ogre::Matrix3 rot;
    rot.FromEulerAnglesXYZ(Ogre::Angle(ToFloat(XmlNode.attribute("angleX").value())), Ogre::Angle(ToFloat(XmlNode.attribute("angleY").value())),
                           Ogre::Angle(ToFloat(XmlNode.attribute("angleZ").value())));
    orientation.FromRotationMatrix(rot);
  } else if (XmlNode.attribute("x")) {
    orientation.x = ToFloat(XmlNode.attribute("x").value());
    orientation.y = ToFloat(XmlNode.attribute("y").value());
    orientation.z = ToFloat(XmlNode.attribute("z").value());
    orientation.w = ToFloat(XmlNode.attribute("w").value());
  } else if (XmlNode.attribute("w")) {
    orientation.w = ToFloat(XmlNode.attribute("w").value());
    orientation.x = ToFloat(XmlNode.attribute("x").value());
    orientation.y = ToFloat(XmlNode.attribute("y").value());
    orientation.z = ToFloat(XmlNode.attribute("z").value());
  }

  Ogre::Vector3 direction = orientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
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

Ogre::ColourValue ParseColour(pugi::xml_node &XmlNode) {
  return Ogre::ColourValue(ToFloat(XmlNode.attribute("r").value()), ToFloat(XmlNode.attribute("g").value()), ToFloat(XmlNode.attribute("b").value()),
                           ToFloat(XmlNode.attribute("a").value(), 1));
}

}  // namespace Glue

//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#pragma once
#include <OgreQuaternion.h>
#include <OgreColourValue.h>
#include <pugixml.hpp>

namespace pugi {
class xml_node;
}

namespace xio {

std::string GetAttrib(const pugi::xml_node &xml_node,
					  const std::string &attrib,
					  const std::string &defaultValue = "");

float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib, float defaultValue = 0);

int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib, int defaultValue = 0);

bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib, bool defaultValue = false);

Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node);

Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node);

Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node);

void ParseMaterial(Ogre::Entity *ent, const std::string &material_name);

Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node);

Ogre::ColourValue ParseColour(pugi::xml_node &xml_node);

Ogre::ColourValue ParseProperty(pugi::xml_node &xml_node);

} //namespace

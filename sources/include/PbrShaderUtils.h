// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <OgreMaterial.h>

#include <string>
#include <vector>

#define OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES 4

namespace Ogre {
class Camera;
}

namespace xio {

class Pbr {
 public:
  static void UpdatePbrShadowCaster(const Ogre::MaterialPtr &material);

  static void UpdatePbrParams(const Ogre::MaterialPtr &material);

  static void UpdatePbrShadowReceiver(const Ogre::MaterialPtr &material);

  static void UpdatePbrIbl(const Ogre::MaterialPtr &material, bool active = false);

  static void UpdatePbrParams(const std::string &material);

  static void UpdatePbrIbl(const std::string &material, bool realtime);

  static void UpdatePbrShadowReceiver(const std::string &material);

  static void UpdatePbrShadowCaster(const std::string &material);

  static void Cleanup();
  static void Update(float time);
};

}  // namespace xio

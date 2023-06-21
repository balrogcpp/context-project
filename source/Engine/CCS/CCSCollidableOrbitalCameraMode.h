/* -------------------------------------------------------
Copyright (c) 2012 Alberto G. Salguero (alberto.salguero at uca.es)

Permission is hereby granted, free of charge, to any
person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice
shall be included in all copies or substantial portions of
the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------- */

#pragma once
#ifndef _CollidableOrbitalCameraMode_H_
#define _CollidableOrbitalCameraMode_H_

#include "CCSPrerequisites.h"

#include "CCSOrbitalCameraMode.h"
#include "CCSCameraControlSystem.h"

namespace CCS
{
/**
 * This is basically an attached camera mode where the user
 * can mofify the camera position. If the scene focus is seen
 * as the center of a sphere, the camera rotates arount it
 */
class DllExport CollidableOrbitalCameraMode : public OrbitalCameraMode
{
 public:
  CollidableOrbitalCameraMode(CameraControlSystem* cam, Ogre::Real initialZoom = 1
      , Ogre::Radian initialHorizontalRotation = Ogre::Radian(0), Ogre::Radian initialVerticalRotation = Ogre::Radian(0)
      , bool resetToInitialPosition = true, Ogre::Real collisionmargin = 0.1f)
  : OrbitalCameraMode(cam, initialZoom, initialHorizontalRotation, initialVerticalRotation, resetToInitialPosition, collisionmargin) {}
  virtual ~CollidableOrbitalCameraMode(){};

  virtual bool init();

  virtual void update(const Ogre::Real &timeSinceLastFrame);

  Ogre::Vector3 DefaultCollisionDetectionFunction(Ogre::Vector3 cameraTargetPosition, Ogre::Vector3 cameraPosition);

  // Specific methods
 protected:


};

}

#endif

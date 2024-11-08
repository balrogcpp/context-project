/// created by Andrey Vasiliev

#pragma once
#include <OgreRoot.h>

inline bool RenderSystemIsGL() {
  static bool result = Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem";
  return result;
}
inline bool RenderSystemIsGL3() {
  static bool result = Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem";
  return result;
}
inline bool RenderSystemIsGLES2() {
  static bool result = Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem";
  return result;
}
inline bool RenderSystemIsD3D9() {
  static bool result = Ogre::Root::getSingleton().getRenderSystem()->getName() == "Direct3D9 Rendering Subsystem";
  return result;
}
inline bool RenderSystemIsD3D11() {
  static bool result = Ogre::Root::getSingleton().getRenderSystem()->getName() == "Direct3D11 Rendering Subsystem";
  return result;
}

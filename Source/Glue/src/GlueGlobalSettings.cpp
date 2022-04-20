// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "GlueGlobalSettings.h"
#include "Engine.h"
#include <OgrePlatformInformation.h>

using namespace std;
using namespace Ogre;

namespace Glue {

bool GlobalMRTIsEnabled() { return true; }

bool CPUSupportsSSE() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE2() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE2);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE3() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE3);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE41() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE41);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE42() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE42);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsNEON() {
#if OGRE_CPU == OGRE_CPU_ARM
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_NEON);
  return Result;
#else
  return false;
#endif
};

bool IsFullscreen() { return GetEngine().IsFullscreen(); }

bool RenderSystemIsGL() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL Rendering Subsystem");
  return Result;
};

bool RenderSystemIsGL3() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL 3+ Rendering Subsystem");
  return Result;
};

bool RenderSystemIsGLES2() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL ES 2.x Rendering Subsystem");
  return Result;
};

SceneManager* OgreSceneManager() {
  static SceneManager* Result = Ogre::Root::getSingleton().getSceneManager("Default");
  return Result;
}

SceneNode* OgreRootNode() {
  static SceneNode* Result = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  return Result;
}

Camera* OgreCamera() {
  static Camera* Result = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  return Result;
}

SceneNode* OgreCameraNode() {
  static SceneNode* Result = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  return Result;
}

Vector3 SunDirection() {
  Vector3 Result = -OgreSceneManager()->getLight("Sun")->getParentSceneNode()->getPosition();
  return Result;
}

}  // namespace Glue

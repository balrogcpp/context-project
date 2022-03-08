// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "GlueGlobalSettings.h"
#include <OgrePlatformInformation.h>

using namespace std;
using namespace Ogre;

namespace Glue {

bool GlobalMRTEnabled() {
  static bool Result = !RenderSystemGLES2();
  return Result;
}

bool CPUSupportSSE() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportSSE2() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE2);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportSSE3() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE3);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportSSE41() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE41);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportSSE42() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE42);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportNEON() {
#if OGRE_CPU == OGRE_CPU_ARM
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_NEON);
  return Result;
#else
  return false;
#endif
};

bool RenderSystemGL() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL Rendering Subsystem");
  return Result;
};

bool RenderSystemGL3() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL 3+ Rendering Subsystem");
  return Result;
};

bool RenderSystemGLES2() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL ES 2.x Rendering Subsystem");
  return Result;
};

}  // namespace Glue

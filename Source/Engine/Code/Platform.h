/// created by Andrey Vasiliev

#pragma once
#include <OgreComponents.h>
#include <OgrePlatform.h>
#include <OgrePlatformInformation.h>
#include <OgreRoot.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WINDOWS
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define ANDROID
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define APPLE
#define DARWIN
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#define IOS
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#define LINUX
#elif OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
#define EMSCRIPTEN
#endif

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#define MSVC
#elif OGRE_COMPILER == OGRE_COMPILER_GNUC
#define GNUC
#elif OGRE_COMPILER == OGRE_COMPILER_CLANG
#define CLANG
#endif

#if defined(ANDROID) || defined(IOS)
#define MOBILE
#endif

#if defined(WINDOWS) || defined(APPLE) || defined(LINUX)
#define DESKTOP
#endif

#if defined(APPLE) || defined(LINUX)
#define UNIX
#endif

#if defined(__MINGW32__)
#define MINGW
#endif

#if defined(MOBILE) || defined(EMSCRIPTEN)
#define GLSLES
#else
#define GLSL
#endif

#if defined(OGRE_BUILD_RENDERSYSTEM_GL) || defined(OGRE_BUILD_RENDERSYSTEM_GL3PLUS) || defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
#define OGRE_OPENGL
#endif

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

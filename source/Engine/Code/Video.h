/// created by Andrey Vasiliev

#pragma once

#include "System.h"
#include <Ogre.h>

inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };

inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };

inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };

inline Ogre::SceneManager* OgreSceneManager() { return Ogre::Root::getSingleton().getSceneManager("Default"); }

inline Ogre::SceneNode* OgreRootNode() { return OgreSceneManager()->getRootSceneNode(); }

inline Ogre::Camera* OgreCamera() { return OgreSceneManager()->getCamera("Default"); }

inline Ogre::SceneNode* OgreCameraNode() { return OgreCamera()->getParentSceneNode(); }

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
bool CheckGL3Version(int major, int minor);
#endif

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
bool CheckGLES2Version(int major, int minor);
#endif

#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
bool CheckGLVersion(int major, int minor);
#endif

namespace Glue {}

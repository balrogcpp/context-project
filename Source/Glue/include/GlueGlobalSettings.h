// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once

namespace Ogre {
class SceneNode;
class SceneManager;
class Camera;
// class Vector3;
}  // namespace Ogre

namespace Glue {

///
bool GlobalMRTIsEnabled();

///
bool CPUSupportsSSE();

///
bool CPUSupportsSSE2();

///
bool CPUSupportsSSE3();

///
bool CPUSupportsSSE41();

///
bool CPUSupportsSSE42();

///
bool CPUSupportsNEON();

///
bool IsFullscreen();

///
bool RenderSystemIsGL();

///
bool RenderSystemIsGL3();

///
bool RenderSystemIsGLES2();

///
Ogre::SceneManager* OgreSceneManager();

///
Ogre::SceneNode* OgreRootNode();

///
Ogre::Camera* OgreCamera();

///
Ogre::SceneNode* OgreCameraNode();

///
Ogre::Vector3 SunDirection();

}  // namespace Glue

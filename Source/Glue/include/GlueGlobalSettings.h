// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once

#include <string>

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

///
int WindowSizeX();

///
int WindowSizeY();

///
std::string WindowCaption();

///
void ShowMouseCursor(bool draw);

}  // namespace Glue

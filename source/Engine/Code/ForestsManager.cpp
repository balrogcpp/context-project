/// created by Andrey Vasiliev

#include "pch.h"
#include "ForestsManager.h"
#include <Ogre.h>

using namespace std;

namespace Glue {
ForestsManager::ForestsManager() {}
ForestsManager::~ForestsManager() {}

void ForestsManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  OgreAssert(ogreRoot, "[ForestsManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  OgreAssert(ogreSceneManager, "[ForestsManager] ogreSceneManager is not initialised");
  OgreAssert(ogreSceneManager->hasCamera("Default"), "[ForestsManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
}
void ForestsManager::OnUpdate(float time) {}
void ForestsManager::OnClean() {}
}

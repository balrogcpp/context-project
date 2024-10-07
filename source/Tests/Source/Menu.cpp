/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "SystemLocator.h"

using namespace std;

namespace gge {

void Menu::OnClean() { GetComponent<SystemLocator>().OnClean(); }

void Menu::OnKeyEvent(SDL_Scancode key, bool pressed) {}

void Menu::OnSetUp() {
  //auto *scene = GetComponent<SceneManager>().GetOgreScene();
  //auto *lNode = scene->getRootSceneNode()->createChildSceneNode("Sun");
  //auto *light = scene->createLight("Sun");
  //light->setType(Ogre::Light::LT_DIRECTIONAL);
  //lNode->attachObject(light);
  //light->setPowerScale(2);
  //light->setCastShadows(true);
  //lNode->setDirection({40.659888, -40.0, -30.950829});
  //light->name

  //auto *rootNode = sceneManager->getRootSceneNode();
  //rootNode->loadChildren(filename);

  //GetComponent<SceneManager>().LoadFromFile("1.scene");
}

void Menu::OnUpdate(float time) {}

void Menu::OnSizeChanged(int x, int y, uint32_t id) {}

}  // namespace gge

//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "DemoDotAppState.h"
#include "MenuAppState.h"
#include "Renderer.h"
#include "Sound.h"

using namespace std;
using namespace xio;

namespace Demo {
DemoDotAppState::DemoDotAppState() {
////  loader_->GetCamera().SetStyle(xio::CameraMan::FPS);
//  LoadFromFile("1.scene");
//
//  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
//  auto *root = scene->getRootSceneNode();
//
//  Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5.0);
//  auto *ps = scene->createParticleSystem("Smoke", "Examples/Smoke");
//  root->createChildSceneNode(Ogre::Vector3(2, 0, 0))->attachObject(ps);
//
////  Ogre::Entity *entity = scene->createEntity("ely_vanguardsoldier_kerwinatienza_Mesh.mesh", "ely_vanguardsoldier_kerwinatienza_Mesh.mesh");
////  auto *node = root->createChildSceneNode(Ogre::Vector3(0, 0, 0));
////  node->scale(Ogre::Vector3(0.02));
////  node->attachObject(entity);
////  UpdateEntityMaterial(entity);
////  anim1 = entity->getAnimationState("run");
////  anim1->setLoop(true);
////  anim1->setEnabled(true);
//
////  anim2 = entity->getAnimationState("jump");
////  anim2->setLoop(true);
////  anim2->setEnabled(true);
//
//  sound_->CreateSound("ambient", "test.ogg", false);
//  sound_->SetVolume("ambient", 0.5);
//  sound_->PlaySound("ambient");
}

DemoDotAppState::~DemoDotAppState() {

}

void DemoDotAppState::Pause() {

}

void DemoDotAppState::Resume() {

}

void DemoDotAppState::OnKeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
	ChangeState();
  } else if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F) {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");

    if (scene->hasLight("Point")) {
      auto *light = scene->getLight("Point");
      light->setVisible(!light->isVisible());
    }
  }
}

void DemoDotAppState::Cleanup() {
}

void DemoDotAppState::Update(float time) {
//  anim1->addTime(time/4);
//  anim2->addTime(time/4);
}

void DemoDotAppState::Init() {
  renderer_->GetWindow().SetCursorStatus(false, true, true);
//  loader_->GetCamera().SetStyle(xio::CameraMan::FPS);
  LoadFromFile("1.scene", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
//  next_ = make_unique<MenuAppState>();
  SetNextState(make_unique<MenuAppState>());

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *root = scene->getRootSceneNode();

//  Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5.0);
//  auto *ps = scene->createParticleSystem("Smoke", "Examples/Smoke");
//  root->createChildSceneNode(Ogre::Vector3(2, 0, 0))->attachObject(ps);

//  Ogre::Entity *entity = scene->createEntity("ely_vanguardsoldier_kerwinatienza_Mesh.mesh", "ely_vanguardsoldier_kerwinatienza_Mesh.mesh");
//  auto *node = root->createChildSceneNode(Ogre::Vector3(0, 0, 0));
//  node->scale(Ogre::Vector3(0.02));
//  node->attachObject(entity);
//  UpdateEntityMaterial(entity);
//  anim1 = entity->getAnimationState("run");
//  anim1->setLoop(true);
//  anim1->setEnabled(true);

//  anim2 = entity->getAnimationState("jump");
//  anim2->setLoop(true);
//  anim2->setEnabled(true);

//  sound_->CreateSound("ambient", "test.ogg", false);
//  sound_->SetVolume("ambient", 0.5);
//  sound_->PlaySound("ambient");
}
}

//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

#include "pcheader.h"
#include "DemoDotAppState.h"

namespace Demo {
using namespace xio;

DemoDotAppState::DemoDotAppState() {}
DemoDotAppState::~DemoDotAppState() {}

void DemoDotAppState::OnKeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
    SwitchNextState(std::make_unique<DemoDotAppState>());
  }
}

void DemoDotAppState::Clear() {
  if (controller_) input_->UnregObserver(controller_.get());
}

void DemoDotAppState::Loop(float time) {
  if (controller_) controller_->Update(time);
}

void DemoDotAppState::Callback(int a, int b) {
}

void DemoDotAppState::Create() {
  loader_->GetCamera().SetStyle(xio::CameraMan::FPS);
  Load("1.scene");

//  controller_ = std::make_unique<SinbadCharacterController>(Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default"));
//  input_->RegObserver(controller_.get());
//  UpdateMeshMaterial("Sinbad.mesh");
//  UpdateMeshMaterial("Sword.mesh");

//  sound_->CreateSound("ambient", "test.ogg", false);
//  sound_->SetVolume("ambient", 0.5);
//  sound_->PlaySound("ambient");
}
}
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
#include "Utils.h"
#include "ComponentLocator.h"

using namespace std;
using namespace xio;

namespace Demo {
DemoDotAppState::DemoDotAppState() {

}

DemoDotAppState::~DemoDotAppState() {

}

//----------------------------------------------------------------------------------------------------------------------
void DemoDotAppState::Pause() {

}

//----------------------------------------------------------------------------------------------------------------------
void DemoDotAppState::Resume() {

}

//----------------------------------------------------------------------------------------------------------------------
void DemoDotAppState::OnKeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym)==SDL_SCANCODE_ESCAPE) {
	context_menu_ = true;
	GetEngine().InMenu();
	GetRender().GetWindow().Grab(false);
  }

}

void DemoDotAppState::Cleanup() {
  Ogre::ImGuiOverlay::NewFrame();
}

static string ButtonText(const std::string &text, int length) {
  string new_string = string((length - text.size())/2, ' ');
  new_string.append(text);
  new_string.append(string((length - text.size())/2, ' '));

  return new_string;
}

//----------------------------------------------------------------------------------------------------------------------
void DemoDotAppState::Update(float time) {
//  anim1->addTime(time/4);
//  anim2->addTime(time/4);


  Ogre::ImGuiOverlay::NewFrame();

  {
	static ImGuiIO &io = ImGui::GetIO();
	ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5);

	ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	ImGui::SetWindowFontScale(0.25);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f/ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
	ImGui::End();

#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
	if (!context_menu_) {
	  GetEngine().OffMenu();
	  return;
	} else {
	  GetEngine().InMenu();
	}
#endif

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x*0.5f, io.DisplaySize.y*0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.5);
	ImGui::SetNextWindowFocus();

#if OGRE_PLATFORM==OGRE_PLATFORM_ANDROID
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);

	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoResize);
#else
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
#endif

	const float hdx = 1920;
	const float hdy = 1080;
	const float hddiag = sqrt(hdx * hdx + hdy * hdy);
	float x = GetWindow().GetSize().first;
	float y = GetWindow().GetSize().second;
	static float diag = sqrt(x * x + y * y);
	float scale = 0.5 * diag / hddiag;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
//	scale *= 2.0f;
#endif
	ImGui::SetWindowFontScale(scale);


#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
	ImGui::NewLine();

	if (ImGui::Button("         RESUME          ")) {
	  GetWindow().Grab(true);
	  GetEngine().OffMenu();
	  context_menu_ = false;
	}
#endif

	ImGui::NewLine();

	if (ImGui::Button("        MAIN MENU        "))
	  ChangeState(make_unique<MenuAppState>());

	ImGui::NewLine();

	if (ImGui::Button("          EXIT           "))
	  ChangeState();

	ImGui::NewLine();

	ImGui::End();
  }

}

//----------------------------------------------------------------------------------------------------------------------
void DemoDotAppState::Init() {
  GetRender().GetWindow().Grab(true);
  GetLoader().GetCamera().SetStyle(xio::CameraMan::Style::FPS);
  LoadFromFile("1.scene", Ogre::RGN_DEFAULT);

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *root = scene->getRootSceneNode();

//  Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5.0);
//  xio::Pbr::UpdatePbrParams("Examples/Smoke");
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

  GetAudio().CreateSound("ambient", "Wind-Mark_DiAngelo-1940285615.ogg", true);
  GetAudio().SetVolume("ambient", 0.5);
  GetAudio().PlaySound("ambient");
}

}

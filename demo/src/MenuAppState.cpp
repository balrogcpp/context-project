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

#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "ComponentLocator.h"

using namespace std;
using namespace xio;

namespace Demo {

void MenuAppState::Cleanup() {
  Ogre::ImGuiOverlay::NewFrame();
}

void MenuAppState::Update(float time) {

  Ogre::ImGuiOverlay::NewFrame();

  {
	static ImGuiIO &io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x*0.5f, io.DisplaySize.y*0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
	ImGui::SetNextWindowFocus();

	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	const float hdx = 1920;
	const float hdy = 1080;
	const float hddiag = sqrt(hdx * hdx + hdy * hdy);
	float x = GetWindow().GetSize().first;
	float y = GetWindow().GetSize().second;
	static float diag = sqrt(x * x + y * y);
	float scale = 0.25f * diag / hddiag;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	scale *= 4.0f;
#endif
	ImGui::SetWindowFontScale(scale);

	ImGui::NewLine();

	if (ImGui::Button("        DEMO        "))
	  ChangeState(make_unique<DemoDotAppState>());

	ImGui::NewLine();

	if (ImGui::Button("        EXIT           "))
	  ChangeState();

	ImGui::NewLine();

	ImGui::End();
  }

}

void MenuAppState::OnKeyDown(SDL_Keycode sym) {

}

void MenuAppState::Init() {
  GetWindow().SetCursorStatus(true, false, false);

  ImGuiIO &io = ImGui::GetIO();
}

}

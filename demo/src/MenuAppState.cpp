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

#define IMGUI_ENABLE_FREETYPE
#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "Renderer.h"
#include <Overlay/OgreImGuiOverlay.h>

using namespace std;
using namespace xio;

namespace Demo {

void MenuAppState::Cleanup() {
  Ogre::ImGuiOverlay::NewFrame();
}

static ImFont *font = nullptr;

void MenuAppState::Update(float time) {


  Ogre::ImGuiOverlay::NewFrame();

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
	static float f = 0.0f;
	static int counter = 0;

	using namespace ImGui;

//	ImGui::ShowDemoWindow();

	static ImGuiIO& io = ImGui::GetIO();
	SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
	SetNextWindowSize({0, 0}, ImGuiCond_Always);
	SetNextWindowCollapsed(false, ImGuiCond_Always);
	SetNextWindowFocus();


	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	ImGui::NewLine();

    if (ImGui::Button("        DEMO        "))
	  ChangeState(make_unique<DemoDotAppState>());

	ImGui::NewLine();

    if (ImGui::Button("        EXIT        "))
	  ChangeState();

	ImGui::NewLine();

	ImGui::End();
  }

}

void MenuAppState::OnKeyDown(SDL_Keycode sym) {
//  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_G) {
//	ChangeState(make_unique<DemoDotAppState>());
//  }
}

void MenuAppState::Init() {
  renderer_->GetWindow().SetCursorStatus(true, false, false);

  ImGuiIO& io = ImGui::GetIO();
  }

}

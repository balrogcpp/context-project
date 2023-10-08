/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "SystemLocator.h"

using namespace std;

namespace gge {
void Menu::OnClean() { GetComponent<SystemLocator>().OnClean(); }

void Menu::OnKeyEvent(SDL_Scancode key, bool pressed) {}

void Menu::OnSetUp() {
  GetComponent<VideoManager>().ShowOverlay(true);
  GetComponent<SceneManager>().LoadFromFile("1.scene");
}

void Menu::OnUpdate(float time) {}

void Menu::OnSizeChanged(int x, int y, uint32_t id) {}

void Menu::BeforeRender(float time) {}
}  // namespace gge

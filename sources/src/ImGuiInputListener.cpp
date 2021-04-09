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

#include "ImGuiInputListener.h"
#include <imgui.h>

using namespace std;

namespace xio {

/// map sdl2 mouse buttons to imgui
static int sdl2imgui(int b) {

  switch (b) {
	case 2: return 2;
	case 3: return 1;
	default: return b - 1;
  }
}

/// SDL2 keycode to scancode
static int kc2sc(int kc) {

  return kc & ~(1 << 30);
}

//----------------------------------------------------------------------------------------------------------------------
static int keypad2kc(int sym, int mod) {

  if (sym < SDLK_KP_1 || sym > SDLK_KP_PERIOD)
	return sym;
  bool numlock = (mod & KMOD_NUM)!=0;
  if (numlock)
	return sym;
  if (sym==SDLK_KP_1)
	sym = SDLK_END;
  else if (sym==SDLK_KP_2)
	sym = SDLK_DOWN;
  else if (sym==SDLK_KP_3)
	sym = SDLK_PAGEDOWN;
  else if (sym==SDLK_KP_4)
	sym = SDLK_LEFT;
  else if (sym==SDLK_KP_6)
	sym = SDLK_RIGHT;
  else if (sym==SDLK_KP_7)
	sym = SDLK_HOME;
  else if (sym==SDLK_KP_8)
	sym = SDLK_UP;
  else if (sym==SDLK_KP_9)
	sym = SDLK_PAGEUP;
  else if (sym==SDLK_KP_0)
	sym = SDLK_INSERT;
  else if (sym==SDLK_KP_PERIOD)
	sym = SDLK_DELETE;
  return sym;
}

//----------------------------------------------------------------------------------------------------------------------
ImGuiInputListener::ImGuiInputListener() {
  static ImGuiIO &io = ImGui::GetIO();

  // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will
  // update during the application lifetime.
  io.KeyMap[ImGuiKey_Tab] = '\t';
  io.KeyMap[ImGuiKey_LeftArrow] = kc2sc(SDLK_LEFT);
  io.KeyMap[ImGuiKey_RightArrow] = kc2sc(SDLK_RIGHT);
  io.KeyMap[ImGuiKey_UpArrow] = kc2sc(SDLK_UP);
  io.KeyMap[ImGuiKey_DownArrow] = kc2sc(SDLK_DOWN);
  io.KeyMap[ImGuiKey_PageUp] = kc2sc(SDLK_PAGEUP);
  io.KeyMap[ImGuiKey_PageDown] = kc2sc(SDLK_PAGEDOWN);
  io.KeyMap[ImGuiKey_Home] = kc2sc(SDLK_HOME);
  io.KeyMap[ImGuiKey_End] = kc2sc(SDLK_END);
  io.KeyMap[ImGuiKey_Insert] = kc2sc(SDLK_INSERT);
  io.KeyMap[ImGuiKey_Delete] = kc2sc(SDLK_DELETE);
  io.KeyMap[ImGuiKey_Backspace] = '\b';
  io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
  io.KeyMap[ImGuiKey_Escape] = '\033';
  io.KeyMap[ImGuiKey_Space] = ' ';
  io.KeyMap[ImGuiKey_KeyPadEnter] = kc2sc(SDLK_KP_ENTER);
  io.KeyMap[ImGuiKey_A] = 'a';
  io.KeyMap[ImGuiKey_C] = 'c';
  io.KeyMap[ImGuiKey_V] = 'v';
  io.KeyMap[ImGuiKey_X] = 'x';
  io.KeyMap[ImGuiKey_Y] = 'y';
  io.KeyMap[ImGuiKey_Z] = 'z';
}

//----------------------------------------------------------------------------------------------------------------------
ImGuiInputListener::~ImGuiInputListener() {

}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnKeyDown(SDL_Keycode sym) {
  static auto &io = ImGui::GetIO();
  const int MODE = KMOD_NUM;

  sym = keypad2kc(sym, MODE);
  int key = kc2sc(sym);
  io.KeysDown[key] = true;
}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnKeyUp(SDL_Keycode sym) {
  static auto &io = ImGui::GetIO();
  int mod = KMOD_NUM;

  sym = keypad2kc(sym, mod);
  int key = kc2sc(sym);
  io.KeysDown[key] = false;
}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnTextInput(const char *text) {
  static auto &io = ImGui::GetIO();

  io.AddInputCharactersUTF8(text);
}

//Mouse

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseMove(int dx, int dy) {


}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  static auto &io = ImGui::GetIO();

  io.MousePos.x = static_cast<float>(x);
  io.MousePos.y = static_cast<float>(y);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T> static int sign(T val) {
  return (T(0) < val) - (val < T(0));
}

void ImGuiInputListener::OnMouseWheel(int x, int y) {
  static auto &io = ImGui::GetIO();

  io.MouseWheel = static_cast<float>(sign(y));
}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseLbDown(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_LEFT);
  if (b < 5) {
	io.MouseDown[b] = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseLbUp(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_LEFT);
  if (b < 5) {
	io.MouseDown[b] = false;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseRbDown(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_RIGHT);
  if (b < 5) {
	io.MouseDown[b] = true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseRbUp(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_RIGHT);
  if (b < 5) {
	io.MouseDown[b] = false;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseMbDown(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_MIDDLE);
  if (b < 5) {
	io.MouseDown[b] = true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void ImGuiInputListener::OnMouseMbUp(int x, int y) {
  static auto &io = ImGui::GetIO();

  int b = sdl2imgui(SDL_BUTTON_MIDDLE);
  if (b < 5) {
	io.MouseDown[b] = false;
  }

}

} //namespace

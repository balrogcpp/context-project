/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include "Singleton.h"
#include <SDL2/SDL_keycode.h>
#include <imgui.h>

namespace Glue {
/// \class to control ImGui interface overlay
class ImGuiListener final : public KeyboardListener, public MouseListener, public ControllerListener, public Singleton<ImGuiListener> {
 public:
  /// map sdl2 mouse buttons to imgui
  inline int sdl2imgui(int b) noexcept {
    switch (b) {
      case 2:
        return 2;
      case 3:
        return 1;
      default:
        return b - 1;
    }
  }

  /// SDL2 keycode to scancode
  inline int kc2sc(int kc) noexcept { return kc & ~(1 << 30); }

  ///
  inline int keypad2kc(int sym, int mod) noexcept {
    if (sym < SDLK_KP_1 || sym > SDLK_KP_PERIOD) return sym;
    bool numlock = (mod & KMOD_NUM) != 0;
    if (numlock) return sym;
    if (sym == SDLK_KP_1)
      sym = SDLK_END;
    else if (sym == SDLK_KP_2)
      sym = SDLK_DOWN;
    else if (sym == SDLK_KP_3)
      sym = SDLK_PAGEDOWN;
    else if (sym == SDLK_KP_4)
      sym = SDLK_LEFT;
    else if (sym == SDLK_KP_6)
      sym = SDLK_RIGHT;
    else if (sym == SDLK_KP_7)
      sym = SDLK_HOME;
    else if (sym == SDLK_KP_8)
      sym = SDLK_UP;
    else if (sym == SDLK_KP_9)
      sym = SDLK_PAGEUP;
    else if (sym == SDLK_KP_0)
      sym = SDLK_INSERT;
    else if (sym == SDLK_KP_PERIOD)
      sym = SDLK_DELETE;
    return sym;
  }

  ImGuiListener() {
    static ImGuiIO &io = ImGui::GetIO();

    // Keyboard mapping. ImGui will use those indices to peek into the
    // io.KeyDown[] array that we will update during the application lifetime.
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
    io.KeyMap[ImGuiKey_Enter] = kc2sc(SDLK_RETURN);
    io.KeyMap[ImGuiKey_Escape] = '\033';
    io.KeyMap[ImGuiKey_Space] = ' ';
    io.KeyMap[ImGuiKey_KeypadEnter] = kc2sc(SDLK_KP_ENTER);
    io.KeyMap[ImGuiKey_A] = 'a';
    io.KeyMap[ImGuiKey_C] = 'c';
    io.KeyMap[ImGuiKey_V] = 'v';
    io.KeyMap[ImGuiKey_X] = 'x';
    io.KeyMap[ImGuiKey_Y] = 'y';
    io.KeyMap[ImGuiKey_Z] = 'z';
  }

  virtual ~ImGuiListener() {}

  inline void OnKeyDown(SDL_Keycode sym) noexcept {
    static auto &io = ImGui::GetIO();
    const int MODE = KMOD_NUM;

    sym = keypad2kc(sym, MODE);
    int key = kc2sc(sym);
    io.KeysDown[key] = true;
  }

  inline void OnKeyUp(SDL_Keycode sym) noexcept {
    static auto &io = ImGui::GetIO();
    int mod = KMOD_NUM;

    sym = keypad2kc(sym, mod);
    int key = kc2sc(sym);
    io.KeysDown[key] = false;
  }

  inline void OnTextInput(const char *text) noexcept {
    static auto &io = ImGui::GetIO();
    io.AddInputCharactersUTF8(text);
  }

  // Mouse
  inline void OnMouseMove(int dx, int dy) noexcept {}

  inline void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) noexcept {
    static auto &io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(x);
    io.MousePos.y = static_cast<float>(y);
  }

  template <typename T>
  inline int sign(T val) noexcept {
    return (T(0) < val) - (val < T(0));
  }

  inline void OnMouseWheel(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    io.MouseWheel = static_cast<float>(sign(y));
  }

  inline void OnMouseLbDown(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_LEFT);
    if (b < 5) io.MouseDown[b] = true;
  }

  inline void OnMouseLbUp(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_LEFT);
    if (b < 5) io.MouseDown[b] = false;
  }

  inline void OnMouseRbDown(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_RIGHT);
    if (b < 5) io.MouseDown[b] = true;
  }

  inline void OnMouseRbUp(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_RIGHT);
    if (b < 5) io.MouseDown[b] = false;
  }

  inline void OnMouseMbDown(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_MIDDLE);
    if (b < 5) io.MouseDown[b] = true;
  }

  inline void OnMouseMbUp(int x, int y) noexcept {
    static auto &io = ImGui::GetIO();
    int b = sdl2imgui(SDL_BUTTON_MIDDLE);
    if (b < 5) io.MouseDown[b] = false;
  }
};
}  // namespace Glue

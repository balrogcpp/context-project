/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include <OgrePrerequisites.h>

struct SDL_Window;

namespace gge {
class Window : public WindowListener {
 public:
  Window();
  virtual ~Window();
  void Create(const std::string& title, Ogre::Camera* camera, int display, int width, int height, uint32_t sdlFlags = 0);
  void Show(bool show);
  void Delete();
  void RenderFrame() const;
  void SetCaption(const char* caption);
  void SetIcon(const char* icon);
  void SetSize(int x, int y);
  void SetFullscreen(bool fullscreen);
  void SetMaximized();
  void SetMinimized();
  void SetRestored();
  void SetPrevSize();
  void SetRaised();
  void SetRefresh();
  void SetPosition(int x, int y, int display = -1);
  void SetDisplay(int display);
  void SetPositionCentered(int display = -1);
  void SetGrabMouse(bool grab);
  void SetMouseRelativeMode(bool relative);
  void SetShowCursor(bool show);
  void SetBordered(bool bordered);
  void SetResizable(bool resizable);
  void SetAlwaysOnTop(bool alwayOnTop);

  int GetSizeX();
  int GetSizeY();
  int GetPositionX();
  int GetPositionY();
  bool IsFullscreen();
  bool IsBordered();
  uint32_t GetID();
  int GetDisplay();
  int GetDisplaySizeX();
  int GetDisplaySizeY();
  float GetDisplayDPI();
  float GetDisplayHDPI();
  float GetDisplayVDPI();

  std::string GetCaption();

  void EnableVsync(bool enable);
  void SetVsyncInterval(int interval);
  bool IsVsyncEnabled();


 protected:
  friend class VideoManager;

  /// WindowListener impl
  void OnEvent(const SDL_Event& event) override;
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;
  void OnSizeChanged(int x, int y, uint32_t id) override;

  std::string title;
  bool fullscreen;
  int display;
  int sizeX;
  int sizeY;
  uint32_t sdlFlags;
  int vsyncInt;
  uint32_t id;

  SDL_Window* sdlWindow = nullptr;
  SDL_GLContext glContext = nullptr;
  Ogre::Root* ogreRoot = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::RenderTarget* renderTarget = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  Ogre::Camera* camera = nullptr;
};
}  // namespace gge

/// created by Andrey Vasiliev

#pragma once

#include "SDLListener.h"
#include <Ogre.h>

struct SDL_Window;

namespace Glue {
class Window : public WindowListener {
 public:
  Window();
  virtual ~Window();
  void Create(const std::string& caption, Ogre::Camera* camera, int monitor, int width, int height, uint32_t sdlFlags);
  void Show(bool show);
  void Delete();
  void RenderFrame() const;
  void GrabCursor(bool grab);
  void ShowCursor(bool show);
  void Resize(int width, int height);
  void SetFullscreen(bool fullscreen);
  void SetWindowCaption(const char* caption);

 protected:
  friend class VideoManager;

  void OnEvent(const SDL_Event& event) override;
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;
  void OnSizeChanged(int x, int y, uint32_t id) override;
  void OnExposed() override;

  std::string caption;
  bool fullscreen;
  int width;
  int height;
  uint32_t sdlFlags;
  bool vsync;
  uint32_t id;
  Ogre::Root* ogreRoot = nullptr;
  SDL_Window* sdlWindow = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::RenderTarget* renderTarget = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
};
}  // namespace Glue

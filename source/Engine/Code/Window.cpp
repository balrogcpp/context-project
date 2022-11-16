/// created by Andrey Vasiliev

#include "pch.h"
#include "Window.h"
#include "Platform.h"
#include <Ogre.h>
#include <SDL2/SDL_syswm.h>

using namespace std;

namespace Glue {
Window::Window()
    : sdlFlags(SDL_WINDOW_HIDDEN), vsync(true), sizeX(1270), display(0), sizeY(720), fullscreen(false), id(0), screenWidth(0), screenHeight(0) {
#ifdef EMSCRIPTEN
  sdlFlags |= SDL_WINDOW_RESIZABLE;
#endif
#ifdef MOBILE
  fullscreen = true;
#endif
}
Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void Window::Create(const string &title, Ogre::Camera *camera, int display, int width, int height, uint32_t flags) {
  this->title = title;
  this->sizeX = width;
  this->sizeY = height;
  this->ogreCamera = camera;
  sdlFlags = flags;
  SDL_DisplayMode displayMode;

  // select biggest display
  if (display < 0) {
    for (unsigned int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
      if (!SDL_GetCurrentDisplayMode(i, &displayMode)) {
        string buff = "Display #" + to_string(i) + ": " + SDL_GetDisplayName(i) + " " + to_string(displayMode.w) + "x" + to_string(displayMode.h) +
                      " @" + to_string(displayMode.refresh_rate);
        Ogre::LogManager::getSingleton().logMessage(buff);
        int screenDiag = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
        int screenDiagI = sqrt(displayMode.w * displayMode.w + displayMode.h * displayMode.h);
        if (screenDiagI > screenDiag) {
          screenWidth = displayMode.w;
          screenHeight = displayMode.h;
          this->display = i;
        }
      }
    }
  } else if (SDL_GetCurrentDisplayMode(display, &displayMode) == 0) {
    screenWidth = displayMode.w;
    screenHeight = displayMode.h;
    this->display = display;
    string buff = "Display #" + to_string(display) + ": " + SDL_GetDisplayName(display) + " " + to_string(displayMode.w) + "x" +
                  to_string(displayMode.h) + " @" + to_string(displayMode.refresh_rate);
    Ogre::LogManager::getSingleton().logMessage(buff);
  }

  if (sdlFlags & SDL_WINDOW_FULLSCREEN) {
    fullscreen = true;
  }

  if (fullscreen) {
    sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    sdlFlags |= SDL_WINDOW_BORDERLESS;
    sizeX = screenWidth;
    sizeY = screenHeight;
  }

#if defined(DESKTOP)
  int32_t sdlPositionFlags = SDL_WINDOWPOS_CENTERED_DISPLAY(this->display);
  sdlWindow = SDL_CreateWindow(title.c_str(), sdlPositionFlags, sdlPositionFlags, sizeX, sizeY, sdlFlags);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  sdlFlags |= SDL_WINDOW_OPENGL;
  sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, sdlFlags);
  SDL_GL_CreateContext(sdlWindow);
#endif

  ASSERTION(sdlWindow, "SDL_CreateWindow failed");
  Ogre::NameValuePairList renderParams;
  const char *TRUE_STR = "true";
  const char *FALSE_STR = "false";
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdlWindow, &info);

  renderParams["vsync"] = vsync ? TRUE_STR : FALSE_STR;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  renderParams["currentGLContext"] = TRUE_STR;
  renderParams["externalGLControl"] = TRUE_STR;
  renderParams["preserveContext"] = TRUE_STR;
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
#endif
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "ogreRoot not initialised");
  ogreWindow = ogreRoot->createRenderWindow(title, sizeX, sizeY, fullscreen, &renderParams);
  renderTarget = ogreRoot->getRenderTarget(ogreWindow->getName());
  ogreViewport = renderTarget->addViewport(ogreCamera);

  // android is not completely ok without it
  SetSize(sizeX, sizeY);

  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  ogreCamera->setAutoAspectRatio(true);
  id = SDL_GetWindowID(sdlWindow);
}

void Window::Show(bool show) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  show ? SDL_ShowWindow(sdlWindow) : SDL_HideWindow(sdlWindow);
}

void Window::SetSize(int x, int y) {
  sizeX = x;
  sizeY = y;
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowSize(sdlWindow, x, y);
  ogreWindow->resize(x, y);
}

void Window::SetFullscreen(bool fullscreen) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  this->fullscreen = fullscreen;

  if (fullscreen) {
    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
    SDL_SetWindowFullscreen(sdlWindow, 0);
  }
}

void Window::SetMaximized() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_MaximizeWindow(sdlWindow);
}

void Window::SetMinimized() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_MinimizeWindow(sdlWindow);
}

void Window::SetRestored() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_RestoreWindow(sdlWindow);
}

void Window::SetPosition(int x, int y, int display) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  if (display < 0) {
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(this->display), SDL_WINDOWPOS_CENTERED_DISPLAY(this->display));
    SDL_SetWindowPosition(sdlWindow, x, y);
  } else {
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
    SDL_SetWindowPosition(sdlWindow, x, y);
  }
}

void Window::SetDisplay(int display) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  if (display > 0 && display != this->display)
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
}

void Window::SetPositionCentered(int display) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  if (display < 0) {
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(this->display), SDL_WINDOWPOS_CENTERED_DISPLAY(this->display));
  } else {
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
  }
}

void Window::SetCaption(const char *caption) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  this->title = caption;
  SDL_SetWindowTitle(sdlWindow, caption);
}

void Window::SetIcon(const char *icon) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  // SDL_SetWindowIcon(sdlWindow, icon);
}

void Window::SetGrabMouse(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
#endif
}

void Window::SetMouseRelativeMode(bool relative) {
#ifndef MOBILE  // This breaks input @Android >9.0
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(relative));
#endif
}

void Window::SetShowCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  show ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
#endif
}

void Window::SetBordered(bool bordered) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowBordered(sdlWindow, static_cast<SDL_bool>(bordered));
}

void Window::SetResizable(bool resizable) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowResizable(sdlWindow, static_cast<SDL_bool>(resizable));
}

void Window::SetAlwaysOnTop(bool alwayOnTop) {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowAlwaysOnTop(sdlWindow, static_cast<SDL_bool>(alwayOnTop));
}

int Window::GetSizeX() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  int x = 0, y = 0;
  SDL_GetWindowSize(sdlWindow, &x, &y);
  return x;
}

int Window::GetSizeY() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  int x = 0, y = 0;
  SDL_GetWindowSize(sdlWindow, &x, &y);
  return y;
}

int Window::GetPositionX() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  int x = 0, y = 0;
  SDL_GetWindowPosition(sdlWindow, &x, &y);
  return y;
}

int Window::GetPositionY() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  int x = 0, y = 0;
  SDL_GetWindowPosition(sdlWindow, &x, &y);
  return y;
}

uint32_t Window::GetID() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  return SDL_GetWindowID(sdlWindow);
}

bool Window::IsFullscreen() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  return static_cast<bool>(SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_FULLSCREEN);
}

bool Window::IsBordered() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  return !static_cast<bool>(SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_BORDERLESS);
}

std::string Window::GetCaption() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  return std::string(SDL_GetWindowTitle(sdlWindow));
}

void Window::Delete() {}

void Window::RenderFrame() const {
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(sdlWindow);
#endif
}

void Window::OnEvent(const SDL_Event &event) {}
void Window::OnQuit() {}
void Window::OnFocusLost() {}
void Window::OnFocusGained() {}
void Window::OnSizeChanged(int x, int y, uint32_t id) {
  if (this->id == id) {
    sizeX = x;
    sizeY = y;
    ogreWindow->resize(x, y);
  }
}
}  // namespace Glue

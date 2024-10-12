/// created by Andrey Vasiliev

#include "pch.h"
#include "Window.h"
#include "Platform.h"
#include <Ogre.h>
#include <SDL2/SDL_syswm.h>
#if defined(__ANDROID__)
#define MANUAL_GL_CONTROL
#endif

using namespace std;

inline static void ParseSDLError(bool result, const char *message = "") {
  if (!result) LogError(message, SDL_GetError());
}

namespace gge {

Window::Window()
    : sdlFlags(SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE),
      vsyncInt(1),
      sizeX(1270),
      display(0),
      sizeY(720),
      fullscreen(false),
      id(0) {
#ifdef MOBILE
  vsyncInt = 2;
  fullscreen = true;
#endif
}
Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, 0); }

void Window::Create(const string &title, Ogre::Camera *camera, int display, int width, int height, uint32_t flags) {
  this->title = title;
  this->sizeX = width;
  this->sizeY = height;
  this->camera = camera;
  //sdlFlags = flags;
  SDL_DisplayMode displayMode;
  int screenWidth = 0, screenHeight = 0;

  // select biggest display
  if (display < 0) {
    for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
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
  } else if (!SDL_GetCurrentDisplayMode(display, &displayMode)) {
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

#ifdef MANUAL_GL_CONTROL
  if (RenderSystemIsGL3()) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  } else if (RenderSystemIsGLES2()) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
#ifndef MOBILE
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#endif
  }

  sdlFlags |= SDL_WINDOW_OPENGL;
  sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sizeX, sizeY, sdlFlags);
  ParseSDLError(sdlWindow, "SDL_CreateWindow failed");
  ASSERTION(sdlWindow, "SDL_CreateWindow failed");

  glContext = SDL_GL_CreateContext(sdlWindow);
  ParseSDLError(glContext, "SDL_GLContext is null");
  ASSERTION(glContext, "SDL_GLContext is null");
#else
  sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sizeX, sizeY, sdlFlags);
  ParseSDLError(sdlWindow, "SDL_CreateWindow failed");
  ASSERTION(sdlWindow, "SDL_CreateWindow failed");
#endif

  Ogre::NameValuePairList renderParams;
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdlWindow, &info);
  renderParams["gama"] = "false";
  renderParams["FSAA"] = "0";
  renderParams["preserveContext"] = "true";
#ifdef MANUAL_GL_CONTROL
  renderParams["currentGLContext"] = "true";
  renderParams["externalGLControl"] = "true";
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
#endif

  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "ogreRoot not initialised");
  ogreWindow = ogreRoot->createRenderWindow("Main", sizeX, sizeY, fullscreen, &renderParams);
#ifdef MANUAL_GL_CONTROL
  SDL_GL_SetSwapInterval(vsyncInt);
#else
  ogreWindow->setVSyncEnabled(vsyncInt != 0);
  ogreWindow->setVSyncInterval(vsyncInt);
#endif
  renderTarget = ogreRoot->getRenderTarget("Main");
  ogreViewport = renderTarget->addViewport(camera);

  // android is not completely ok without it
#if defined(__ANDROID__)
  SetSize(sizeX, sizeY);
#endif

  camera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  camera->setAutoAspectRatio(true);
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
  this->fullscreen = fullscreen;
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  fullscreen ? SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_SetWindowFullscreen(sdlWindow, 0);
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

void Window::SetPrevSize() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SetSize(sizeX, sizeY);
}

void Window::SetRaised() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_RaiseWindow(sdlWindow);
}

void Window::SetRefresh() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowTitle(sdlWindow, title.c_str());
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
  if (display >= 0 && display != this->display) {
    if (fullscreen) SDL_SetWindowFullscreen(sdlWindow, 0);
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
    if (fullscreen) SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
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
#if !defined(__ANDROID__) // This breaks input @Android >9.0
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
#endif
}

void Window::SetMouseRelativeMode(bool relative) {
#if !defined(__ANDROID__)  // This breaks input @Android >9.0
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(relative));
#endif
}

void Window::SetShowCursor(bool show) {
#if !defined(__ANDROID__)  // This breaks input @Android >9.0
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

int Window::GetDisplay() {
  ASSERTION(sdlWindow, "sdlWindow not initialised");
  return SDL_GetWindowDisplayIndex(sdlWindow);
}

int Window::GetDisplaySizeX() {
  SDL_DisplayMode displayMode;
  return !SDL_GetCurrentDisplayMode(GetDisplay(), &displayMode) ? displayMode.w : -1;
}

int Window::GetDisplaySizeY() {
  SDL_DisplayMode displayMode;
  return !SDL_GetCurrentDisplayMode(GetDisplay(), &displayMode) ? displayMode.h : -1;
}

float Window::GetDisplayDPI() {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(GetDisplay(), &ddpi, &hdpi, &vdpi);
  return !res ? ddpi : 1.0;
}

float Window::GetDisplayHDPI() {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(GetDisplay(), &ddpi, &hdpi, &vdpi);
  return !res ? hdpi : 1.0;
}

float Window::GetDisplayVDPI() {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(GetDisplay(), &ddpi, &hdpi, &vdpi);
  return !res ? vdpi : 1.0;
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

void Window::EnableVsync(bool enable) {
  ASSERTION(ogreWindow, "ogreWindow not initialised");
  ogreWindow->setVSyncEnabled(enable);
  if (enable) {
#ifdef MANUAL_GL_CONTROL
    SDL_GL_SetSwapInterval(vsyncInt);
#else
    ogreWindow->setVSyncInterval(vsyncInt);
#endif
  } else {
#ifdef MANUAL_GL_CONTROL
    SDL_GL_SetSwapInterval(0);
#else
    ogreWindow->setVSyncInterval(vsyncInt);
#endif
  }
}

void Window::SetVsyncInterval(int interval) {
  ASSERTION(ogreWindow, "ogreWindow not initialised");
  vsyncInt = interval;
#ifdef MANUAL_GL_CONTROL
  SDL_GL_SetSwapInterval(interval);
#else
  ogreWindow->setVSyncInterval(interval);
#endif
}

bool Window::IsVsyncEnabled() {
  ASSERTION(ogreWindow, "ogreWindow not initialised");
  return ogreWindow->isVSyncEnabled();
}

void Window::Delete() {}

void Window::RenderFrame() const {
#ifdef MANUAL_GL_CONTROL
  SDL_GL_SwapWindow(sdlWindow);
#endif
}

void Window::OnEvent(const SDL_Event &event) {}
void Window::OnQuit() {}
void Window::OnFocusLost() {}
void Window::OnFocusGained() {}
void Window::OnSizeChanged(int x, int y, uint32_t id) {
#ifndef MOBILE
  if (this->id == id) {
    ogreWindow->resize(x, y);
  }
#endif
}

}  // namespace gge

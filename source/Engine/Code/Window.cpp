/// created by Andrey Vasiliev

#include "pch.h"
#include "Window.h"
#include "Platform.h"
#include <SDL2/SDL_syswm.h>

using namespace std;

namespace Glue {

Window::Window() : sdlFlags(SDL_WINDOW_HIDDEN), vsync(true), width(1270), height(720), fullscreen(false), id(0) {}

Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void Window::Create(const string &caption, Ogre::Camera *camera, int monitor, int width, int height, uint32_t sdlFlags) {
  this->caption = caption;
  this->width = width;
  this->height = height;
  this->ogreCamera = camera;
  this->sdlFlags = sdlFlags;
  SDL_DisplayMode displayMode;
  int screenWidth = 0, screenHeight = 0, currentDisplay = 0;

  // select biggest display
  if (monitor < 0) {
    for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
      if (SDL_GetCurrentDisplayMode(i, &displayMode) == 0) {
        char buff[500];
        sprintf(buff, "[SDL_Log] Display #%d: current display mode is %dx%dpx @ %dhz", i, displayMode.w, displayMode.h, displayMode.refresh_rate);
        Ogre::LogManager::getSingleton().logMessage(buff);
        int screenDiag = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
        int screenDiagI = sqrt(displayMode.w * displayMode.w + displayMode.h * displayMode.h);
        if (screenDiagI > screenDiag) {
          screenWidth = displayMode.w;
          screenHeight = displayMode.h;
          currentDisplay = i;
        }
      } else {
        char buff[500];
        sprintf(buff, "[SDL_Log] Display #%d: current display mode is %dx%dpx @ %dhz", i, displayMode.w, displayMode.h, displayMode.refresh_rate);
        Ogre::LogManager::getSingleton().logError(buff);
      }
    }
  } else {
    if (SDL_GetCurrentDisplayMode(monitor, &displayMode) == 0) {
      screenWidth = displayMode.w;
      screenHeight = displayMode.h;
      currentDisplay = monitor;
      char buff[500];
      sprintf(buff, "[SDL_Log] Display #%d: current display mode is %dx%dpx @ %dhz", monitor, displayMode.w, displayMode.h, displayMode.refresh_rate);
      Ogre::LogManager::getSingleton().logMessage(buff);
    } else {
      char buff[500];
      sprintf(buff, "[SDL_Log] Display #%d: current display mode is %dx%dpx @ %dhz", monitor, displayMode.w, displayMode.h, displayMode.refresh_rate);
      Ogre::LogManager::getSingleton().logError(buff);
    }
  }

  if (sdlFlags & SDL_WINDOW_FULLSCREEN) fullscreen = true;

#if defined(DESKTOP)
  if (width == screenWidth && height == screenHeight) {
    sdlFlags |= SDL_WINDOW_BORDERLESS;
  }
  if (fullscreen) {
    sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    sdlFlags |= SDL_WINDOW_BORDERLESS;
    width = screenWidth;
    height = screenHeight;
  }
  int32_t sdlPositionFlags = SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay);
  sdlWindow = SDL_CreateWindow(caption.c_str(), sdlPositionFlags, sdlPositionFlags, width, height, sdlFlags);
  OgreAssert(sdlWindow, "SDL_CreateWindow failed");
  id = SDL_GetWindowID(sdlWindow);
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
#ifdef EMSCRIPTEN
  sdlFlags |= SDL_WINDOW_RESIZABLE;
#endif
  sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#ifdef MOBILE
  sdlFlags |= SDL_WINDOW_BORDERLESS;
  sdlFlags |= SDL_WINDOW_FULLSCREEN;
#endif
  width = screenWidth;
  height = screenHeight;
  int32_t sdlPositionFlags = SDL_WINDOWPOS_CENTERED;
  sdlWindow = SDL_CreateWindow(nullptr, sdlPositionFlags, sdlPositionFlags, screenWidth, screenHeight, sdlFlags);
  SDL_GL_CreateContext(sdlWindow);
#endif

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
  OgreAssert(ogreRoot, "ogreRoot not initialised");
  ogreWindow = ogreRoot->createRenderWindow(caption, width, height, fullscreen, &renderParams);
  renderTarget = ogreRoot->getRenderTarget(ogreWindow->getName());
  ogreViewport = renderTarget->addViewport(ogreCamera);
  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  ogreCamera->setAutoAspectRatio(true);
#ifdef ANDROID
  SDL_GetDesktopDisplayMode(currentDisplay, &displayMode);
  ogreWindow->resize(static_cast<int>(displayMode.w), static_cast<int>(displayMode.h));
#endif
}

void Window::Show(bool show) { show ? SDL_ShowWindow(sdlWindow) : SDL_HideWindow(sdlWindow); }

void Window::Resize(int width, int height) {
  width = width;
  height = height;
  SDL_SetWindowSize(sdlWindow, width, height);
  ogreWindow->resize(width, height);
  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
}

void Window::GrabCursor(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  OgreAssert(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
  grab ? SDL_ShowCursor(SDL_DISABLE) : SDL_ShowCursor(SDL_ENABLE);
#endif
}

void Window::ShowCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  OgreAssert(sdlWindow, "sdlWindow not initialised");
  show ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
#endif
}

void Window::SetWindowCaption(const char *caption) {
  this->caption = caption;
  SDL_SetWindowTitle(sdlWindow, caption);
}

void Window::SetFullscreen(bool fullscreen) {
  if (fullscreen) {
    fullscreen = true;
    ogreWindow->setFullscreen(fullscreen, width, height);
#ifdef DESKTOP
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    fullscreen = false;
    ogreWindow->setFullscreen(fullscreen, width, height);
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags);
    SDL_SetWindowSize(sdlWindow, width, height);
  }
}

void Window::Delete() {}

void Window::RenderFrame() const {
  ogreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
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
    width = x;
    height = y;
    ogreWindow->resize(x, y);
  }
#endif
}
void Window::OnExposed() {}

}  // namespace Glue

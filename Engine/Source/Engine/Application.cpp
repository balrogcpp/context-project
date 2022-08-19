// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "Engine.h"
#include <OgreFileSystemLayer.h>
#ifndef MOBILE
#if __has_include(<filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else  // APPLE
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif  // !APPLE
#endif  // DESKTOP
extern "C" {
#ifdef _MSC_VER
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
}

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <windows.h>
#elif defined(UNIX)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif


using namespace std;


namespace Glue {

Application::Application() {
  InputSequencer::GetInstance().RegWinObserver(this);
  EnginePtr = make_unique<Engine>();
  EnginePtr->InitComponents();
}

Application::~Application() { InputSequencer::GetInstance().UnregWinObserver(this); }

void Application::LoopBody() {
  auto before_frame = chrono::system_clock::now().time_since_epoch();
  int64_t TimeBeforeFrame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

  if (CumultedTime > int64_t(1e+6)) CumultedTime = 0;

  EnginePtr->Capture();

  if (!Suspend) {
    if (WasSuspended) {
      EnginePtr->OnResume();
      WasSuspended = false;
    }

    auto before_update = chrono::system_clock::now().time_since_epoch();
    int64_t TimeBeforeUpdate = chrono::duration_cast<chrono::microseconds>(before_update).count();
    float frame_time = static_cast<float>(TimeBeforeUpdate - TimeOfLastFrame) / 1e+6;
    TimeOfLastFrame = TimeBeforeUpdate;
    EnginePtr->Update(frame_time);
    EnginePtr->RenderFrame();
  } else {
    EnginePtr->OnPause();
    WasSuspended = true;
  }

  auto TimeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  auto RenderTime = TimeAftetRender - TimeBeforeFrame;

  if (LockFPS) {
    auto delay = static_cast<int64_t>((1e+6 / TargetFPS) - RenderTime);
    if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
  }

  int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  int64_t TimeSinceLastFrame = TimeInEndOfLoop - TimeBeforeFrame;
  CumultedTime += TimeSinceLastFrame;

#ifdef EMSCRIPTEN
  if (!Running) emscripten_cancel_main_loop();
#endif
}

void Application::Loop() {
  while (Running) {
    LoopBody();
  }
}

#ifdef EMSCRIPTEN
void Application::EmscriptenLoop(void *arg) {
  Application *App = static_cast<Application *>(arg);
  App->LoopBody();
}
#endif

void Application::Go() {
  Running = true;
  auto duration_before_update = chrono::system_clock::now().time_since_epoch();
  TimeOfLastFrame = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *root = scene->getRootSceneNode();
  root->loadChildren("1.scene");


  //if (Running) {
    //EnginePtr->RenderFrame();
    // ClearRTSSRuntime();
  //}

#ifdef MOBILE
  TargetFPS = 30;
#endif
#ifndef EMSCRIPTEN
  Loop();
#else
  LockFPS = false;
  emscripten_set_main_loop_arg(Application::EmscriptenLoop, GetInstancePtr(), 0, 1);
#endif

  EnginePtr->OnCleanup();
  EnginePtr->OnPause();
}

void Application::OnQuit() { Running = false; }

void Application::OnPause() {
  Suspend = true;
  EnginePtr->OnPause();
}

void Application::OnResume() {
  Suspend = false;
  EnginePtr->OnResume();
}


int Application::Main() {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
  SDL_SetMainReady();
#endif
  ios_base::sync_with_stdio(false);
  Go();
  SDL_Quit();
  return 0;
}

}  // namespace Glue

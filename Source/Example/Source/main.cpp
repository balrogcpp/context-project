// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
extern "C" {
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
}
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;
using namespace Glue;

int ErrorWindow(const string &WindowCaption, const string &MessageText) {
  static Engine *EnginePtr = Engine::GetInstancePtr();
  if (EnginePtr) EnginePtr->GrabCursor(false);

    SDL_Log("%s", string(WindowCaption + " : " + MessageText).c_str());

#ifdef _WIN32
  MessageBox(nullptr, MessageText.c_str(), WindowCaption.c_str(), MB_ICONERROR);
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WindowCaption.c_str(), MessageText.c_str(), nullptr);
#endif

#ifdef EMSCRIPTEN
  emscripten_pause_main_loop();
#endif

  return 1;
}

#if defined(_WIN32) && !defined(DEBUG)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
  try {
    // main app class object
    Application app;
    // main function
    return app.Main();

  } catch (exception &e) {
    return ErrorWindow("Exception", e.what());
  } catch (...) {
    return ErrorWindow("Exception", "unknown");
  }

  // to be sure app will return something to system
  return 0;
}

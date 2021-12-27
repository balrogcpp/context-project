// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Exception.h"
#include "Engine.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

namespace Glue {

int ErrorWindow(const string &WindowCaption, const string &MessageText) {
  SDL_Log("%s", string(WindowCaption + " : " + MessageText).c_str());

#ifdef _WIN32
  MessageBox(nullptr, MessageText.c_str(), WindowCaption.c_str(), MB_ICONERROR);
#else
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WindowCaption.c_str(), MessageText.c_str(), nullptr);
#endif

  return 1;
}

void Assert(bool Result, std::string Message) {
  if (!Result) {
    throw Exception(std::move(Message));
  }
}

void Throw(std::string Message) { throw Exception(std::move(Message)); }

}  // namespace Glue

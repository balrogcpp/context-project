//MIT License
//
//Copyright (c) 2021 Andrew Vasilev
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

#include "DemoDotAppState.h"
#include "MenuAppState.h"
#include "DesktopApplication.h"

using namespace std;
using namespace xio;

#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
#if defined WIN32 && defined WINAPI_MAIN_FUNC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

INT WINAPI WinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
#elif OGRE_PLATFORM==OGRE_PLATFORM_LINUX || OGRE_PLATFORM==OGRE_PLATFORM_APPLE
int main(int argc, char **argv)
#endif
{
  DesktopApplication app;
  app.Main(make_unique<Demo::MenuAppState>());

  return 0;
}

#else
extern "C" {
  #include <android_native_app_glue.h>
}

void handle_cmd(android_app *pApp, int32_t cmd) {
//      switch (cmd) {
//        case APP_CMD_INIT_WINDOW:
//            pApp->userData = new Renderer(pApp);
//            break;
//
//        case APP_CMD_TERM_WINDOW:
//            if (pApp->userData) {
//                auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);
//                pApp->userData = nullptr;
//                delete pRenderer;
//            }
//    }
}


void android_main(struct android_app *pApp) {
  pApp->onAppCmd = handle_cmd;

  int events;
  android_poll_source *pSource;
  do {
	if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
	  if (pSource) {
		pSource->process(pApp, pSource);
	  }
	}
  } while (!pApp->destroyRequested);
}

#endif

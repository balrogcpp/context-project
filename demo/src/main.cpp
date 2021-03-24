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
#include "Application.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;
using namespace xio;

#if defined WIN32 && defined WINAPI_MAIN_FUNC
INT WINAPI WinMain
(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
extern "C" {
#include <android_native_app_glue.h>
#include <jni.h>
#include <android/log.h>

void android_main(struct android_app* state);
}

void android_main(struct android_app* state)
#else
int main(int argc, char** argv)
#endif
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  SetProcessDPIAware();
#endif

  try {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    Application app;
#else
	Application app(state);
#endif
    app.Main(make_unique<Demo::MenuAppState>());
  }
  catch (...) {

  }

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  return 0;
#endif
}

//
// Created by Patrick Martin on 1/30/19.
//

//#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
//extern "C" {
//#include <android_native_app_glue.h>
//#include <jni.h>
//#include <android/log.h>
//
//void android_main(struct android_app* state);
//void handle_cmd(android_app *pApp, int32_t cmd) {
//}
//
//void android_main(struct android_app *pApp) {
//  pApp->onAppCmd = handle_cmd;
//
//  int events;
//  android_poll_source *pSource;
//  do {
//	if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
//	  if (pSource) {
//		pSource->process(pApp, pSource);
//	  }
//	}
//  } while (!pApp->destroyRequested);
//}
//}
//#endif

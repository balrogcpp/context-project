//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#include "pcheader.h"
#include "DesktopApplication.h"
#include "Overlay.h"
#include "DesktopIcon.h"
#include "Exception.h"
#include "SDL2.hpp"
#include "VerboseListener.h"

#ifdef _WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace xio {
DesktopApplication::DesktopApplication() {

}

//----------------------------------------------------------------------------------------------------------------------
DesktopApplication::~DesktopApplication() {

}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::Quit() {
  running_ = false;
}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::Event(const SDL_Event &evt) {
  if (evt.type==SDL_WINDOWEVENT) {
	if (evt.window.event==SDL_WINDOWEVENT_FOCUS_LOST || evt.window.event==SDL_WINDOWEVENT_MINIMIZED) {
	  suspend_ = true;
//		renderer_->GetWindow().SetCursorStatus(true, false, false);
	  state_manager_->Pause();
	} else if (evt.window.event==SDL_WINDOWEVENT_FOCUS_GAINED) {
	  suspend_ = false;
//		renderer_->GetWindow().SetCursorStatus(false, true, true);
	  state_manager_->Resume();
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::Other(uint8_t type, int32_t code, void *data1, void *data2) {

}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::OnLoop() {

}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::OnGo() {

}

//----------------------------------------------------------------------------------------------------------------------
void DesktopApplication::OnMain() {
	auto verbose_listener = make_unique<VerboseListener>();
	//verbose_listener->verbose = true;

	verbose_ = conf_->Get<bool>("global_verbose");
	lock_fps_ = conf_->Get<bool>("global_lock_fps");
	target_fps_ = conf_->Get<int>("global_target_fps");

#if OGRE_PLATFORM==OGRE_PLATFORM_LINUX
  DesktopIcon icon;
  icon.Init();
  icon.Save("XioDemo");
#endif

}

} //namespace

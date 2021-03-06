//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
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

#pragma once

#include "NoCopy.h"
#include "Input.h"
#include "ComponentLocator.h"

namespace xio {
class StateManager;

class AppState
	: public Ogre::RenderTargetListener,
	  public Ogre::FrameListener,
	  public InputObserver,
	  public NoCopy,
	  public ComponentLocator {
 public:
  AppState();
  virtual ~AppState();

  void ChangeState(std::unique_ptr<AppState> &&app_state);
  void ChangeState();
//  std::unique_ptr<AppState> GetNextState();
  void SetNextState(std::unique_ptr<AppState> &&next_state);
  void LoadFromFile(const std::string &file_name);
  bool IsDirty() const;

  virtual void Init() = 0;
  virtual void Cleanup() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void Update(float time) = 0;

 protected:
  friend class StateManager;
  std::unique_ptr<AppState> next_;
  bool dirty_ = false;
};
}

/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "ManagerCommon.hpp"

namespace Forests {
class PagedGeometry;
}
namespace Context {

class PagedForestManager : public ManagerCommon {
 public:
  static PagedForestManager *GetSingletonPtr() {
    return &pagedForestManagerSingleton;
  }

  static PagedForestManager &GetSingleton() {
    return pagedForestManagerSingleton;
  }

 private:
  static PagedForestManager pagedForestManagerSingleton;

 public:
  void Create();

  bool frameRenderingQueued(const Ogre::FrameEvent &evt) final;

  void Reset() final;

 private:
  const float GRASS_WIDTH = 0.5f;
  const float GRASS_HEIGHT = 0.5f;
  Forests::PagedGeometry *trees = nullptr;
  Forests::PagedGeometry *plants = nullptr;
  Forests::PagedGeometry *grass = nullptr;
};

} //Context

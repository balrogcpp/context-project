//Cpp file for dummy context2_deps target
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

#include "pcheader.h"
#include "Field.h"
#include "PbrUtils.h"
#include "PagedGeometry.h"
using namespace Forests;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Field::Field() = default;

Field::~Field() {
  if (Ogre::MeshManager::getSingleton().getByName("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
    Ogre::MeshManager::getSingleton().remove("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
std::function<float(float, float)> Field::heigh_func_;
void Field::Create() {
  PagedGeometry *grass = new PagedGeometry(Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default"), 50);
  grass->addDetailLevel<GrassPage>(100);//Draw grass up to 100
  GrassLoader *grassLoader = new GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  grassLoader->setHeightFunction([](float x, float z, void*){return Ogre::Real(heigh_func_(x, z) - 0.1);});
  UpdatePbrParams("GrassCustom");
  UpdatePbrShadowReceiver("GrassCustom");
  GrassLayer *layer = grassLoader->addLayer("GrassCustom");
  layer->setFadeTechnique(FADETECH_GROW);
  layer->setRenderTechnique(GRASSTECH_CROSSQUADS);
  layer->setMinimumSize(2.0f, 2.0f);
  layer->setAnimationEnabled(true);
  layer->setSwayDistribution(10.0f);
  layer->setSwayLength(1.0f);
  layer->setSwaySpeed(0.5f);
  layer->setDensity(3.0f);
  layer->setMapBounds(TBounds(-100, -100, 100, 100));
  layer->setDensityMap("terrain2.png");
  layer->setColorMap("terrain_lightmap.jpg");
  grass->update();
}
}
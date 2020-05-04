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

const bool g_enablePagedGeometry = false;
const bool g_enableFog = true;
const bool g_saveTerrain = false;

const float g_worldSize = 200.0f; //4000.0f
const float g_worldBounds = (g_worldSize / 2.0f);
const float g_seaLevel = 1300.0f;
const int g_numberOfTrees = 200; //1200
const float g_treeDistance = 100;
const float g_treeImposterDistance = 150.0f;
const float g_treeWindFactor = 7.0f;
const float g_grassDistance = 100.0f;
const float g_grassDensity = 0.1f;
const float g_plantWindFactor = 3.0f;
const float g_plantDistance = 500.0f;
const float g_plantsImposterDistance = 500.0f;
const float g_numberOfPlants = 500;
const float g_cloudOpacity = 1.0f;
const float g_cloudSpeed = 100.0f;
const float g_cloudFallX = 0;
const float g_cloudFallY = 0;
const float g_fogStartDist = 500;
const float g_fogEndDist = 250.0f;
const float g_pageSize_Grass = 20.0f;
const float g_pageSize_Plants = 50.0f;
const float g_pageSize_Trees = 50;
const float g_compositeMapDistance = g_enableFog ? g_fogEndDist + 1000.0f : g_worldSize;

//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.vert"

in vec4 position;
#ifdef ALPHA
in vec2 uv0;
#endif
uniform mat4 uModelMatrix;
uniform mat4 cWorldViewProj;
uniform mat4 cWorldViewProjPrev;
out float distance;
out vec4 vPosition;
out vec4 vPrevPosition;
#ifdef ALPHA
out vec2 vUV;
#endif

void main()
{
#ifdef ALPHA
    vUV = uv0;
#endif

    distance = length((cWorldViewProj * position).xyz);// transform the vertex position to the view space
    vPosition = cWorldViewProj * position;
    vPrevPosition = (cWorldViewProjPrev * uModelMatrix) * position;
    gl_Position = vPosition;
}
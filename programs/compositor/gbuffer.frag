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
#include "header.frag"

in float distance;
in vec4 vPosition;
in vec4 vPrevPosition;
uniform float cNearClipDistance;
uniform float cFarClipDistance;// !!! might be 0 for infinite view projection.
#ifdef HAS_ALPHA
in vec2 vUV;
uniform sampler2D baseColor;
#endif

void main()
{
#ifdef HAS_ALPHA
    if (texture2D(baseColor, vUV).a < 0.5) {
        discard;
    }
#endif

    float clipDistance = cFarClipDistance - cNearClipDistance;
//    gl_FragData[0] = vec4((distance - cNearClipDistance) / clipDistance, 0.0, 0.0, 1.0);

    vec2 a = (vPosition.xz / vPosition.w) * 0.5 + 0.5;
    vec2 b = (vPrevPosition.xz / vPrevPosition.w) * 0.5 + 0.5;
//    gl_FragData[1] = vec4(vec2(a - b), 0.0, 1.0);
    gl_FragColor = vec4((distance - cNearClipDistance) / clipDistance, vec2(a - b), 1.0);
}
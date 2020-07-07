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

#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
out vec4 gl_FragColor;
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#define VERSION 300
#version VERSION es
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#if VERSION == 100
#define in varying
#define out varying
#else
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
out vec4 gl_FragColor;
#endif
#endif

in    vec3 oViewPos;
in    vec3 oNormal;
in     vec2 vUV;

uniform float cNearClipDistance;
uniform float cFarClipDistance;// !!! might be 0 for infinite view projection.
//uniform sampler2D baseColor;

void main()
{
//    vec4 color = texture2D(baseColor, vUV);
//    if (color.a < 1.0) {
//        discard;
//    }

    float distance = length(oViewPos);
    float clipDistance = cFarClipDistance - cNearClipDistance;

    gl_FragColor = vec4((distance - cNearClipDistance) / clipDistance, 0, 0, 1);

//    gl_FragData[0] = vec4(normalize(oNormal).xyz, (distance - cNearClipDistance) / clipDistance);// normal + linear depth [0, 1]
//    gl_FragData[1] = vec4(oViewPos, 0.0);// view space position
}
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
#define attribute in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#version 100
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#define in varying
#define out varying
#endif
#if VERSION != 120
out vec4 gl_FragColor;
#endif

varying vec2 oUv0;

uniform sampler2D sSceneDepthSampler;
uniform sampler2D AttenuationSampler;
uniform sampler2D sRotSampler4x4;
uniform vec4 cViewportSize; // auto param width/height/inv. width/inv. height
uniform float cFov; // vertical field of view in radians
uniform float farClipDistance;
uniform float cSampleInScreenspace; // whether to sample in screen or world space
uniform float cSampleLengthScreenSpace; // The sample length in screen space [0, 1]
uniform float cSampleLengthWorldSpace; // the sample length in world space in units
uniform float cOffsetScale; // [0, 1] The distance of the first sample. samples are the
// placed in [cOffsetScale * cSampleLengthScreenSpace, cSampleLengthScreenSpace]
uniform float cDefaultAccessibility; // the default value used in the lerp() expression for invalid samples [0, 1]
uniform float cEdgeHighlight; // multiplier for edge highlighting in [1, 2] 1 is full highlighting 2 is off

void main()
{
  const int nSampleNum = 32; // number of samples

  vec4 attenuation = texture2D(AttenuationSampler, oUv0);

  // get the depth of the current pixel and convert into world space unit [0, inf]
  float fragmentWorldDepth = texture2D(sSceneDepthSampler, oUv0).w * farClipDistance;

  // get rotation vector, rotation is tiled every 4 screen pixels
  vec2 rotationTC = oUv0 * cViewportSize.xy / 4.0;
  vec3 rotationVector = 2.0 * texture2D(sRotSampler4x4, rotationTC).xyz - 1.0; // [-1, 1]x[-1. 1]x[-1. 1]

  float rUV = 0; // radius of influence in screen space
  float r = 0; // radius of influence in world space
  if (cSampleInScreenspace == 1)
  {
    rUV = cSampleLengthScreenSpace;
    r = tan(rUV * cFov) * fragmentWorldDepth;
  }
  else
  {
    rUV = atan(cSampleLengthWorldSpace / fragmentWorldDepth) / cFov; // the radius of influence projected into screen space
    r = cSampleLengthWorldSpace;
  }

  float sampleLength = cOffsetScale; // the offset for the first sample
  float sampleLengthStep = pow((rUV / sampleLength), 1.0f/nSampleNum);

  float accessibility = 0;
  // sample the sphere and accumulate accessibility
  for (int i = 0; i < (nSampleNum/8); i++)
  {
    for (int x = -1; x <= 1; x += 2)
    for (int y = -1; y <= 1; y += 2)
    for (int z = -1; z <= 1; z += 2)
    {
      //generate offset vector
      vec3 offset = normalize(vec3(x, y, z)) * sampleLength;

      // update sample length
      sampleLength *= sampleLengthStep;

      // reflect offset vector by random rotation sample (i.e. rotating it)
      vec3 rotatedOffset = reflect(offset, rotationVector);

      vec2 sampleTC = oUv0 + rotatedOffset.xy * rUV;

      // read scene depth at sampling point and convert into world space units (m or whatever)
      float sampleWorldDepth = texture2D(sSceneDepthSampler, sampleTC).w * farClipDistance;

      // check if depths of both pixels are close enough and sampling point should affect our center pixel
      float fRangeIsInvalid = clamp((fragmentWorldDepth - sampleWorldDepth) / r, 0.0, 1.0);

      // accumulate accessibility, use default value of 0.5 if right computations are not possible

      accessibility += mix(float(sampleWorldDepth > (fragmentWorldDepth + rotatedOffset.z * r)) , cDefaultAccessibility, fRangeIsInvalid);
    }
  }

  // get average value
  accessibility /= nSampleNum;

  // normalize, remove edge highlighting
  accessibility *= cEdgeHighlight;

  // amplify and saturate if necessary
  gl_FragColor = vec4(attenuation.rgb, attenuation.a * accessibility);
}
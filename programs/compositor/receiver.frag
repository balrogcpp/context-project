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

#define MAX_LIGHTS 5

in vec4 lightSpacePosArray[3];
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D baseColor;
uniform vec4 inverseShadowmapSize0;
uniform vec4 inverseShadowmapSize1;
uniform vec4 inverseShadowmapSize2;
uniform vec4 pssmSplitPoints;
uniform vec4 uShadowColour;


in vec3 vPosition;
in vec2 vUV;
in float depth;
#ifdef FADER_ENABLE
in float fade;
#endif

vec4 offsetSample(vec4 uv, vec2 offset, float invMapSize)
{
  return vec4(uv.xy + offset * invMapSize * uv.w, uv.z, uv.w);
}

float calcDepthShadow_Poisson(sampler2D shadowMap, vec4 uv, vec4 invShadowMapSize)
{
  // 4-sample PCF
  float shadow = 0.0;
  uv.z *= uv.w;
  uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1

  float compare = uv.z;
  int counter = 0;
  const float radius = 1.0;
  const int iterations = 16;

  //  float shadow_depth = texture2DProj(shadowMap, uv).r;
  //  shadow = (shadow_depth > compare) ? 1.0 : 0.0;

  for (float y = -radius; y <= radius; y++)
  for (float x = -radius; x <= radius; x++)
  {
    vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy;
    float depth = texture2D(shadowMap, uv2).r;
    if (depth > compare) {
      counter++;
    }
  }
  shadow = counter / ( (2 * radius + 1 ) * (2*radius + 1));

//  const vec2 poissonDisk16[16] = vec2[](
//  vec2( -0.94201624, -0.39906216 ),
//  vec2( 0.94558609, -0.76890725 ),
//  vec2( -0.094184101, -0.92938870 ),
//  vec2( 0.34495938, 0.29387760 ),
//  vec2( -0.91588581, 0.45771432 ),
//  vec2( -0.81544232, -0.87912464 ),
//  vec2( -0.38277543, 0.27676845 ),
//  vec2( 0.97484398, 0.75648379 ),
//  vec2( 0.44323325, -0.97511554 ),
//  vec2( 0.53742981, -0.47373420 ),
//  vec2( -0.26496911, -0.41893023 ),
//  vec2( 0.79197514, 0.19090188 ),
//  vec2( -0.24188840, 0.99706507 ),
//  vec2( -0.81409955, 0.91437590 ),
//  vec2( 0.19984126, 0.78641367 ),
//  vec2( 0.14383161, -0.14100790 )
//  );

//  for (int i = 0; i < iterations; i++)
//  {
//    vec2 uv2 = uv.xy + poissonDisk16[i] * invShadowMapSize.x;
//    float shadow_depth = texture2D(shadowMap, uv2).r;
//    if (shadow_depth > compare) {
//      counter++;
//    }
//  }
//  shadow = counter / iterations;

//  for (float y = -radius; y <= radius; y++)
//  for (float x = -radius; x <= radius; x++)
//  for (int i = 0; i < iterations; i++)
//  {
//    vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy + poissonDisk16[i] * invShadowMapSize.x;
//    float depth = texture2D(shadowMap, uv2).r;
//    if (depth > compare) {
//      counter++;
//    }
//  }
//  shadow = counter / ( iterations * (2 * radius + 1 ) * (2*radius + 1));

  shadow = clamp(shadow + uShadowColour.r, 0, 1);

  return shadow;
}

float calcPSSMDepthShadow(sampler2D shadowMap0, sampler2D shadowMap1, sampler2D shadowMap2,
vec4 inverseShadowmapSize0, vec4 inverseShadowmapSize1, vec4 inverseShadowmapSize2,
vec4 lsPos0, vec4 lsPos1, vec4 lsPos2, vec4 splits, float camDepth)
{
  if (camDepth > 1000) {
    return 1.0;
  }

  float attenuation = 1.0;

  // calculate shadow
  if (camDepth <= splits.x)
  {
    attenuation = calcDepthShadow_Poisson(shadowMap0, lsPos0, inverseShadowmapSize0);
  }
  else if (camDepth <= splits.y)
  {
    attenuation = calcDepthShadow_Poisson(shadowMap1, lsPos1, inverseShadowmapSize1);
  }
  else
  {
    attenuation = calcDepthShadow_Poisson(shadowMap2, lsPos2, inverseShadowmapSize2);
  }

  return attenuation;
}

void main()
{
  if (texture2D(baseColor, vUV).a < 0.5)
    discard;

  float attenuation = calcPSSMDepthShadow(shadowMap0, shadowMap1, shadowMap2,
  inverseShadowmapSize0, inverseShadowmapSize1, inverseShadowmapSize2,
  lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2],
  pssmSplitPoints, depth);

  gl_FragColor = vec4(0.0, 0.0, 0.0, attenuation);
}
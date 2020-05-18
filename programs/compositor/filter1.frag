#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
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
precision mediump float;
#define in varying
#define out varying
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif

in vec2 oUv0;
uniform vec4 screenSize;
uniform sampler2D AttenuationSampler;


void main()
{
  const int radius = 3;
  const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
  const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
//  const int radius = 5;
//  const float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
//  const float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

  vec4 final_color = texture2D(AttenuationSampler, oUv0) * weight[0];
  for (int i=1; i<radius; i++) {
    final_color += texture2D(AttenuationSampler, (oUv0 + vec2(0.0, offset[i]) * screenSize.w) ) * weight[i];
    final_color += texture2D(AttenuationSampler, (oUv0 - vec2(0.0, offset[i]) * screenSize.w) ) * weight[i];
  }

  gl_FragColor = final_color;
}
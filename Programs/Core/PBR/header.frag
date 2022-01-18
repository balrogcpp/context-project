// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef HEADER_FRAG
#define HEADER_FRAG

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif

#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 4
#endif

#ifndef GL_ES

//GLSL

//#define USE_TEX_LOD

#if VERSION == 330 || VERSION == 400 || VERSION == 400 || VERSION == 410 || VERSION == 420 || VERSION == 430 || VERSION == 440 || VERSION == 450 || VERSION == 460
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
out vec4 FragData[2];
//out vec4 FragColor;
#else
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif

#else

//GLSLES 2.0

#extension GL_OES_standard_derivatives : enable
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#define textureCubeLod textureLodEXT
#endif
precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if VERSION == 300 || VERSION == 310 || VERSION == 320
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
//out vec4 FragData[2];
out vec4 FragColor;
#else
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif
#endif

#endif //HEADER_FRAG

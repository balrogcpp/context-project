// created by Andrey Vasiliev

#ifndef HEADER_FRAG
#define HEADER_FRAG

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif

#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 4
#endif

#define MRT
#define USE_TEX_LOD

#ifndef GL_ES

#if __VERSION__ >= 150
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else  // __VERSION__ < 150
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif // __VERSION__ >= 150

#ifdef USE_MRT
out vec4 FragData[3];
#else // NO_MRT
#define NO_MRT
out vec4 FragColor;
#endif // ! NO_MRT

#else // GLSLES

#extension GL_OES_standard_derivatives : enable
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : enable
#endif

precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

#if __VERSION__ >= 300
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else // __VERSION__ < 300
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif // __VERSION__ >= 300

#ifdef USE_MRT
out vec4 FragData[3];
#else // NO_MRT
#define NO_MRT
out vec4 FragColor;
#endif // !NO_MRT

#endif // GLSL_ES

#endif //HEADER_FRAG

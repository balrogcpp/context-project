#ifndef GL_ES
#define VERSION 120
#version VERSION
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
precision highp float;
#define in varying
#define out varying
#endif

uniform samplerCube cubemap;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate

#if VERSION != 120
out vec4 gl_FragColor;
#endif

void main()
{
    gl_FragColor = textureCube(cubemap, TexCoords);
}

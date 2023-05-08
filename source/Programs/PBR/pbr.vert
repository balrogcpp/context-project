// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif


#include "header.glsl"
#include "math.glsl"
#ifdef PAGED_GEOMETRY
#include "pgeometry.glsl"
#endif


#ifndef VERTEX_COMPRESSION
attribute highp vec4 vertex;
#else
attribute highp vec2 vertex;
uniform highp mat4 posIndexToObjectSpace;
uniform highp float baseUVScale;
#endif // VERTEX_COMPRESSION
#ifdef HAS_NORMALS
attribute highp vec4 normal;
attribute highp vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
attribute mediump vec4 colour;
#endif
#ifdef HAS_UV
#ifndef VERTEX_COMPRESSION
attribute highp vec4 uv0;
#else
attribute highp float uv0;
#endif // VERTEX_COMPRESSION
attribute highp vec4 uv1;
attribute highp vec4 uv2;
attribute highp vec4 uv3;
attribute highp vec4 uv4;
attribute highp vec4 uv5;
attribute highp vec4 uv6;
attribute highp vec4 uv7;
#endif //  HAS_UV

varying highp vec3 vWorldPosition;
varying highp float vDepth;
varying highp mat3 vTBN;
varying highp vec2 vUV0;
varying mediump vec4 vColor;
varying mediump vec4 vScreenPosition;
varying mediump vec4 vPrevScreenPosition;
#ifdef SHADOWRECEIVER
varying highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp float MovableObj;
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec3 CameraPosition;
uniform highp float FadeRange;
#endif // PAGED_GEOMETRY
#ifdef SHADOWRECEIVER
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef HAS_VERTEXCOLOR
    vColor = max3(colour.rgb) > 0.0 ? colour.rgba : vec4(1.0, 1.0, 1.0, 1.0);
#else
    vColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif // HAS_VERTEXCOLOR

#ifndef VERTEX_COMPRESSION
    highp vec4 position = vertex;
    highp vec2 uv = uv0.xy;
#else
    highp vec4 position = posIndexToObjectSpace * vec4(vertex, uv0, 1.0);
    highp vec2 uv = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

#ifdef HAS_UV 
    vUV0.xy = uv.xy;
#else
    vUV0.xy = vec2(0.0, 0.0);
#endif // HAS_UV

#ifdef PAGED_GEOMETRY
     position +=  uv2.x == 0.0 ? bigger(0.5, uv0.y) * WaveGrass(position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0)) : WaveTree(position, Time.x, uv1, uv2);
#endif // PAGED_GEOMETRY

#ifdef HAS_NORMALS
    highp vec3 n = normalize(vec3(mul(WorldMatrix, vec4(normal.xyz, 0.0))));
    highp vec3 t = normalize(vec3(mul(WorldMatrix, vec4(tangent.xyz, 0.0))));
    highp vec3 b = cross(n, t) * tangent.w;
    vTBN = mtxFromCols3x3(t, b, n);
#else
    highp vec3 n = vec3(0.0, 1.0, 0.0);
    highp vec3 b = normalize(cross(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0)));
    highp vec3 t = vec3(1.0, 0.0, 0.0);
    vTBN = mtxFromCols3x3(t, b, n);
#endif // HAS_NORMALS

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;

    gl_Position = mul(WorldViewProjMatrix, position);
    vScreenPosition = gl_Position;
    vDepth = gl_Position.z;
    vPrevScreenPosition = MovableObj > 0.0 ? mul(WorldViewProjPrev, position) : mul(WorldViewProjPrev, mul(WorldMatrix, position));

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
    // Calculate the position of vertex attribute light space
    for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
        vLightSpacePosArray[i] = mul(TexWorldViewProjMatrixArray[i], position);
    }
#endif // MAX_SHADOW_TEXTURES > 0
#endif // SHADOWRECEIVER
}

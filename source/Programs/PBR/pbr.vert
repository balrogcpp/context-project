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

#define HAS_MRT
#include "header.glsl"
#ifdef PAGED_GEOMETRY
#include "pgeometry.glsl"
#endif

#ifdef VERTEX_COMPRESSION
uniform highp mat4 posIndexToObjectSpace;
uniform highp float baseUVScale;
#endif
uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp float MovableObj;
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec3 CameraPosition;
uniform highp float FadeRange;
#endif // PAGED_GEOMETRY
#if MAX_SHADOW_TEXTURES > 0
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif

MAIN_PARAMETERS
#ifndef VERTEX_COMPRESSION
IN(highp vec4 vertex, POSITION)
#else
IN(highp vec2 vertex, POSITION)
#endif // VERTEX_COMPRESSION
#ifdef HAS_NORMALS
IN(highp vec4 normal, NORMAL)
IN(highp vec4 tangent, TANGENT)
#endif
#ifdef HAS_VERTEXCOLOR
IN(highp vec4 colour, COLOR)
#endif
#ifdef HAS_UV
#ifndef VERTEX_COMPRESSION
IN(highp vec4 uv0, TEXCOORD0)
#else
IN(highp float uv0, TEXCOORD0)
#endif // VERTEX_COMPRESSION
#ifdef PAGED_GEOMETRY
IN(highp vec4 uv1, TEXCOORD1)
IN(highp vec4 uv2, TEXCOORD2)
#endif // PAGED_GEOMETRY
#endif //  HAS_UV

OUT(highp vec3 vWorldPosition, TEXCOORD0)
OUT(highp float vDepth, TEXCOORD1)
OUT(highp mat3 vTBN, TEXCOORD2)
OUT(highp mat3 vTBN1, TEXCOORD3)
OUT(highp vec2 vUV0, TEXCOORD4)
OUT(mediump vec4 vColor, TEXCOORD5)
OUT(mediump vec4 vScreenPosition, TEXCOORD6)
OUT(mediump vec4 vPrevScreenPosition, TEXCOORD7)
#if MAX_SHADOW_TEXTURES > 0
OUT(highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES], TEXCOORD8)
#endif

MAIN_DECLARATION
{
#ifdef HAS_VERTEXCOLOR
    vColor = max3(colour.rgb) > 0.0 ? colour.rgba : vec4(1.0, 1.0, 1.0, 1.0);
#else
    vColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif // HAS_VERTEXCOLOR

#ifndef VERTEX_COMPRESSION
    highp vec4 position = vertex.xyzw;
    highp vec2 uv = uv0.xy;
#else
    highp vec4 position = posIndexToObjectSpace * vec4(vertex.xy, uv0, 1.0);
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
    highp vec3 n = normalize(mul(WorldMatrix, vec4(normal.xyz, 0.0)).xyz);
    highp vec3 t = normalize(mul(WorldMatrix, vec4(tangent.xyz, 0.0)).xyz);
    highp vec3 b = cross(n, t) * tangent.w;
    vTBN = mtxFromCols3x3(t, b, n);
#ifdef HAS_MRT
    highp vec3 n1 = normalize(mul(WorldViewMatrix, vec4(normal.xyz, 0.0)).xyz);
    highp vec3 t1 = normalize(mul(WorldViewMatrix, vec4(tangent.xyz, 0.0)).xyz);
    highp vec3 b1 = cross(n1, t1) * tangent.w;
    vTBN1 = mtxFromCols3x3(t1, b1, n1);
#endif
#else
    const highp vec3 n = vec3(0.0, 1.0, 0.0);
    const highp vec3 t = vec3(1.0, 0.0, 0.0);
    const highp vec3 b = normalize(cross(n, t));
    vTBN = mtxFromCols3x3(t, b, n);
    vTBN1 = vTBN;
#endif // HAS_NORMALS

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;
    vDepth = length(mul(WorldViewMatrix, position).xyz);

    gl_Position = mul(WorldViewProjMatrix, position);
    vScreenPosition = gl_Position;
#ifdef HAS_MRT
    vPrevScreenPosition = MovableObj > 0.0 ? mul(WorldViewProjPrev, position) : mul(WorldViewProjPrev, mul(WorldMatrix, position));
#endif

#if MAX_SHADOW_TEXTURES > 0
    // Calculate the position of vertex attribute light space
    for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
        vLightSpacePosArray[i] = mul(TexWorldViewProjMatrixArray[i], position);
    }
#endif
}

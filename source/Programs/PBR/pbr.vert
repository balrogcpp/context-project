// created by Andrey Vasiliev

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
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec3 CameraPosition;
uniform highp float FadeRange;
#endif // PAGED_GEOMETRY
#if MAX_SHADOW_TEXTURES > 0
uniform float LightCount;
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif

MAIN_PARAMETERS
#ifndef VERTEX_COMPRESSION
IN( highp vec3 vertex, POSITION)
#else
IN( highp vec2 vertex, POSITION)
#endif // VERTEX_COMPRESSION
#ifdef HAS_NORMALS
IN( highp vec3 normal, NORMAL)
#endif
#ifdef HAS_TANGENTS
IN( highp vec4 tangent, TANGENT)
#endif
#ifdef HAS_VERTEXCOLOR
IN( highp vec3 colour, COLOR)
#endif
#ifdef HAS_UV
#ifndef VERTEX_COMPRESSION
IN( highp vec2 uv0, TEXCOORD0)
#else
IN( highp float uv0, TEXCOORD0)
#endif // VERTEX_COMPRESSION
#ifdef PAGED_GEOMETRY
IN( highp vec4 uv1, TEXCOORD1)
IN( highp vec4 uv2, TEXCOORD2)
#endif // PAGED_GEOMETRY
#endif //  HAS_UV

OUT( highp vec3 vPosition, TEXCOORD0)
#ifdef HAS_UV
OUT( highp vec2 vUV0, TEXCOORD1)
#endif
#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
OUT( mediump mat3 vTBN, TEXCOORD2)
#endif
#ifdef HAS_VERTEXCOLOR
OUT( mediump vec3 vColor, TEXCOORD3)
#endif
OUT( mediump vec4 vScreenPosition, TEXCOORD4)
OUT( mediump vec4 vPrevScreenPosition, TEXCOORD5)
#if MAX_SHADOW_TEXTURES > 0
OUT( highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES], TEXCOORD6)
#endif
MAIN_DECLARATION
{
#ifdef HAS_VERTEXCOLOR
    vColor = Any(colour) ? colour : vec3(1.0, 1.0, 1.0);
#endif

#ifndef VERTEX_COMPRESSION
    highp vec4 position = vec4(vertex, 1.0);
    highp vec2 uv = uv0;
#else
    highp vec4 position = posIndexToObjectSpace * vec4(vertex.xy, uv0, 1.0);
    highp vec2 uv = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

#ifdef HAS_UV
    vUV0 = uv;
#endif

#ifdef PAGED_GEOMETRY
     position +=  uv2.x == 0.0 ? fstep(0.5, uv0.y) * WaveGrass(position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0)) : WaveTree(position, Time.x, uv1, uv2);
#endif

#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
    vec3 n = normalize(mul(WorldMatrix, vec4(normal.xyz, 0.0)).xyz);
    vec3 t = normalize(mul(WorldMatrix, vec4(tangent.xyz, 0.0)).xyz);
    vec3 b = cross(n, t) * tangent.w;
    vTBN = mtxFromCols(t, b, n);
#endif
    
    highp vec4 world = mul(WorldMatrix, position);
    vPosition = world.xyz / world.w;
    gl_Position = mul(WorldViewProjMatrix, position);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, position);

#if MAX_SHADOW_TEXTURES > 0
    // Calculate the position of vertex attribute light space
    for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
        if (max(int(LightCount), 3) <= i) break;
        vLightSpacePosArray[i] = mul(TexWorldViewProjMatrixArray[i], position);
    }
#endif
}

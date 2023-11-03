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
//uniform highp vec3 CameraPosition;
uniform highp float FadeRange;
#endif // PAGED_GEOMETRY
#if MAX_SHADOW_TEXTURES > 0
uniform float LightCount;
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif

#ifndef VERTEX_COMPRESSION
in highp vec3 vertex;
#else
in highp vec2 vertex;
#endif // VERTEX_COMPRESSION
#ifdef HAS_NORMALS
in highp vec4 normal;
in highp vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
in highp vec3 colour;
#endif
#ifdef HAS_UV
#ifndef VERTEX_COMPRESSION
in highp vec2 uv0;
#else
in highp float uv0;
#endif // VERTEX_COMPRESSION
#ifdef PAGED_GEOMETRY
in highp vec4 uv1;
in highp vec4 uv2;
#endif // PAGED_GEOMETRY
#endif //  HAS_UV

out highp vec3 vWorldPosition;
out highp mat3 vTBN;
out highp vec2 vUV0;
out vec4 vColor;
out vec4 vScreenPosition;
out vec4 vPrevScreenPosition;
#if MAX_SHADOW_TEXTURES > 0
out highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
void main()
{
#ifdef HAS_VERTEXCOLOR
    vColor = Any(colour) ? vec4(colour, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);
#else
    vColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif // HAS_VERTEXCOLOR

#ifndef VERTEX_COMPRESSION
    highp vec4 position = vec4(vertex, 1.0);
    highp vec2 uv = uv0;
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
     position +=  uv2.x == 0.0 ? fstep(0.5, uv0.y) * WaveGrass(position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0)) : WaveTree(position, Time.x, uv1, uv2);
#endif // PAGED_GEOMETRY

#ifdef HAS_NORMALS
    highp vec3 n = normalize(mul(WorldMatrix, vec4(normal.xyz, 0.0)).xyz);
    highp vec3 t = normalize(mul(WorldMatrix, vec4(tangent.xyz, 0.0)).xyz);
    highp vec3 b = cross(n, t) * tangent.w;
    vTBN = mtxFromCols3x3(t, b, n);
#else
    const highp vec3 n = vec3(0.0, 1.0, 0.0);
    const highp vec3 t = vec3(1.0, 0.0, 0.0);
    const highp vec3 b = normalize(cross(n, t));
    vTBN = mtxFromCols3x3(t, b, n);
#endif // HAS_NORMALS

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;

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

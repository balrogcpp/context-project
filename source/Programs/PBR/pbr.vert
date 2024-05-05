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
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec3 CameraPosition;
uniform highp float FadeRange;
#endif // PAGED_GEOMETRY

#ifndef VERTEX_COMPRESSION
in highp vec4 vertex;
#else
in highp vec2 vertex;
#endif // VERTEX_COMPRESSION
#ifdef HAS_NORMALS
in highp vec4 normal;
#endif
#ifdef HAS_TANGENTS
in highp vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
in highp vec3 colour;
#endif
#ifdef HAS_UV
//#ifndef VERTEX_COMPRESSION
in highp vec2 uv0;
//#else
//in highp float uv0;
//#endif // VERTEX_COMPRESSION
#ifdef PAGED_GEOMETRY
in highp vec4 uv1;
in highp vec4 uv2;
#endif // PAGED_GEOMETRY
#endif //  HAS_UV

out highp vec3 vPosition;
out highp vec3 vPosition1;
#ifdef HAS_UV
out highp vec2 vUV0;
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mediump mat3 vTBN;
#else
out mediump vec3 vNormal;
#endif
#endif
#ifdef HAS_VERTEXCOLOR
out mediump vec3 vColor;
#endif
void main()
{
#ifdef HAS_VERTEXCOLOR
    vColor = Any(colour) ? colour : vec3(1.0, 1.0, 1.0);
#endif

#ifndef VERTEX_COMPRESSION
    highp vec4 position = vertex;
    highp vec2 uv = uv0.xy;
#else
    highp vec4 position = posIndexToObjectSpace * vec4(vertex.xy, uv0.x, 1.0);
    highp vec2 uv = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

#ifdef HAS_UV
    vUV0 = uv;
#endif

#ifdef PAGED_GEOMETRY
    if (uv2.x == 0.0)
        position += fstep(0.5, uv0.y) * WaveGrass(position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0));
    else
        position += WaveTree(position, Time.x, uv1, uv2);
#endif

#ifdef HAS_NORMALS
    mat3 worldFromModelNormalMatrix = mat3(WorldMatrix);
#if 0
    worldFromModelNormalMatrix = mat3(
        normalize(worldFromModelNormalMatrix[0]),
        normalize(worldFromModelNormalMatrix[1]),
        normalize(worldFromModelNormalMatrix[2])
    );
#endif

#ifdef HAS_TANGENTS
    vec3 n = normalize(mul(worldFromModelNormalMatrix, normal.xyz));
    vec3 t = normalize(mul(worldFromModelNormalMatrix, tangent.xyz));
    vec3 b = cross(n, t) * tangent.w;
    vTBN = mtxFromCols(t, b, n);
#else
    vNormal = normalize(mul(worldFromModelNormalMatrix, normal.xyz));
#endif
#endif

    highp vec4 worldPosition = mulMat4x4Float3(WorldMatrix, position.xyz);
    vPosition = worldPosition.xyz / worldPosition.w;
    vPosition1 = position.xyz;
    gl_Position = mulMat4x4Float3(WorldViewProjMatrix, position.xyz);
}

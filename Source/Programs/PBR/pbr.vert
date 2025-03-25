// created by Andrey Vasiliev
//? #version 400

#include "math.glsl"
#ifdef PAGED_GEOMETRY
#include "pgeometry.glsl"
#endif

#ifdef VERTEX_COMPRESSION
uniform highp mat4 posIndexToObjectSpace;
uniform highp float baseUVScale;
#endif
#ifndef SHADOWCASTER
uniform highp mat4 WorldMatrix;
#endif
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

#ifndef SHADOWCASTER
#ifdef HAS_NORMALS
in highp vec4 normal;
#endif
#ifdef HAS_TANGENTS
in highp vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
in highp vec3 colour;
#endif
#endif // SHADOWCASTER

#ifndef VERTEX_COMPRESSION
#ifdef HAS_UV
in highp vec2 uv0;
#endif
#else
in highp float uv0;
#endif // VERTEX_COMPRESSION
#ifdef PAGED_GEOMETRY
in highp vec4 uv1;
in highp vec4 uv2;
#endif // PAGED_GEOMETRY

#if defined(HAS_UV)
out highp vec2 vUV0;
#endif
#ifndef SHADOWCASTER
out highp vec3 vPosition;
out highp vec3 vPosition1;
#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
out mediump mat3 vTBN;
#endif
#if defined(HAS_VERTEXCOLOR)
out mediump vec3 vColor;
#endif
#endif // SHADOWCASTER
void main()
{
#if !defined(VERTEX_COMPRESSION)
    highp vec4 position = vertex;
    highp vec2 uv = uv0;
#else
    highp vec4 position = posIndexToObjectSpace * vec4(vertex.xy, uv0, 1.0);
    highp vec2 uv = vec2(vertex.x * baseUVScale, 1.0 - (vertex.y * baseUVScale));
#endif

#if defined(PAGED_GEOMETRY)
    if (uv2.x == 0.0)
        position += step(0.5, uv0.y) * WaveGrass(position, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0));
    else
        position += WaveTree(position, Time.x, uv1, uv2);
#endif

#if defined(HAS_UV)
    vUV0 = uv;
#endif

    gl_Position = mulMat4x4Float3(WorldViewProjMatrix, position.xyz);

#ifndef SHADOWCASTER

#if defined(HAS_VERTEXCOLOR)
    vColor = (colour != vec3(0.0, 0.0, 0.0)) ? colour : vec3(1.0, 1.0, 1.0);
#endif


#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
    vec3 n = normalize(mulMat3x3Float3(WorldMatrix, normal.xyz));
    vec3 t = normalize(mulMat3x3Float3(WorldMatrix, tangent.xyz));
    vec3 b = cross(n, t);
    vTBN = mat3(t, b, n);
#endif

    highp vec4 wPosition = mulMat4x4Float3(WorldMatrix, position.xyz);
    vPosition = wPosition.xyz / wPosition.w;
    vPosition1 = position.xyz;

#endif
}

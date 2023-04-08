// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif


#include "header.vert"
#include "math.glsl"
#ifdef PAGED_GEOMETRY
#include "pgeometry.glsl"
#endif


in highp vec4 position;
#ifdef HAS_NORMALS
in mediump vec4 normal;
#endif
#ifdef HAS_TANGENTS
in mediump vec4 tangent;
#endif
#ifdef HAS_VERTEXCOLOR
in mediump vec4 colour;
#endif
#ifdef HAS_UV
in mediump vec4 uv0;
in mediump vec4 uv1;
in mediump vec4 uv2;
in mediump vec4 uv3;
in mediump vec4 uv4;
in mediump vec4 uv5;
#endif //  HAS_UV

out highp vec3 vModelPosition;
out highp vec3 vViewPosition;
out mediump vec2 vUV0;
out mediump float vDepth;
out mediump vec4 vColor;
out mediump vec4 vScreenPosition;
out mediump vec4 vPrevScreenPosition;
out mediump mat3 vTBN;
#ifdef SHADOWRECEIVER
out mediump vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif

uniform highp mat4 ModelMatrix;
uniform highp mat4 ViewMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform mediump float MovableObj;
#ifdef PAGED_GEOMETRY
uniform highp vec4 Time;
uniform highp vec4 CameraPosition;
uniform mediump float FadeRange;
#endif // PAGED_GEOMETRY
#ifdef SHADOWRECEIVER
uniform mediump mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif // SHADOWRECEIVER


//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifdef HAS_UV
    vUV0.xy = uv0.xy;
#endif // HAS_UV

#ifdef HAS_VERTEXCOLOR
    vColor = max3(colour.rgb) > 0.0 ? colour.rgba : vec4(1.0);
#else
    vColor = vec4(1.0);
#endif // HAS_VERTEXCOLOR

    highp vec4 vertex = position;
    highp vec4 model = ModelMatrix * vertex;
    vModelPosition = model.xyz / model.w;

#ifdef PAGED_GEOMETRY
     vertex +=  uv2.x == 0.0 ? bigger(0.5, uv0.y) * WaveGrass(vertex, Time.x, 1.0, vec4(0.5, 0.1, 0.25, 0.0)) : WaveTree(vertex, Time.x, uv1, uv2);
#endif // PAGED_GEOMETRY

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
    highp vec3 n = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
    highp vec3 t = normalize(vec3(ModelMatrix * vec4(tangent.xyz, 0.0)));
    highp vec3 b = cross(n, t) * tangent.w;
    vTBN = mat3(t, b, n);
#else
    highp vec3 n = normalize(vec3(ModelMatrix * vec4(normal.xyz, 0.0)));
    highp vec3 b = normalize(cross(n, vec3(1.0, 0.0, 0.0)));
    highp vec3 t = normalize(cross(n, b));
    vTBN = mat3(t, b, n);
#endif // HAS_TANGENTS
#else
    vTBN = mat3(vec3(1.0, 0.0, 0.0),
                normalize(cross(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0))),
                vec3(0.0, 1.0, 0.0));
#endif // HAS_NORMALS

    highp vec4 view = ViewMatrix * model;
    vViewPosition = view.xyz / view.w;
    gl_Position = WorldViewProjMatrix * vertex;
    vScreenPosition = gl_Position;
    vDepth = gl_Position.z;
    vPrevScreenPosition = MovableObj > 0.0 ? WorldViewProjPrev * vertex : WorldViewProjPrev * ModelMatrix * vertex;

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
    // Calculate the position of vertex in light space
    for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
        vLightSpacePosArray[i] = TexWorldViewProjMatrixArray[i] * vertex;
    }
#endif // MAX_SHADOW_TEXTURES > 0
#endif // SHADOWRECEIVER
}

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
#ifndef MAX_WATER_OCTAVES
    #define MAX_WATER_OCTAVES 3
#endif

SAMPLER2D(NoiseMap, 0);

OGRE_UNIFORMS_BEGIN
uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform mediump vec4 Time;
uniform mediump vec2 BigWaves;
uniform mediump vec2 MidWaves;
uniform mediump vec2 SmallWaves;
uniform mediump float Visibility;
uniform mediump vec2 WindDirection;
uniform mediump float WindSpeed;
uniform mediump float WaveScale;
uniform mediump float ScatterAmount;
uniform mediump vec3 ScatterColor;
uniform mediump float ReflDistortionAmount;
uniform mediump float RefrDistortionAmount;
uniform mediump float AberrationAmount;
uniform mediump vec3 WaterExtinction;
uniform mediump vec3 SunTransmittance;
uniform mediump float SunFade;
uniform mediump float ScatterFade;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(highp vec4 vertex, POSITION)
OUT(highp vec3 vWorldPosition, TEXCOORD0)
OUT(mediump vec4 vScreenPosition, TEXCOORD1)
OUT(mediump vec4 vPrevScreenPosition, TEXCOORD2)
MAIN_DECLARATION
{
    highp vec4 position = vertex;

    highp vec4 world = mul(WorldMatrix, position);
    vWorldPosition = world.xyz / world.w;

    mediump vec2 nCoord = vWorldPosition.xz * WaveScale * 0.04 + WindDirection * Time.x * WindSpeed * 0.04;
    mediump float normal0 = texture2D(NoiseMap, nCoord + vec2(-Time.x * 0.015, -Time.x * 0.005)).x;
    highp float height = normal0 * BigWaves.x;

#if MAX_WATER_OCTAVES > 0
    nCoord = vWorldPosition.xz * WaveScale * 0.1 + WindDirection * Time.x * WindSpeed * 0.08;
    mediump float normal1 = texture2D(NoiseMap, nCoord + vec2(Time.x * 0.020, Time.x * 0.015)).x;
    height += normal1 * BigWaves.y;
#endif
#if MAX_WATER_OCTAVES > 1
    nCoord = vWorldPosition.xz * WaveScale * 0.25 + WindDirection * Time.x * WindSpeed * 0.07;
    mediump float normal2 = texture2D(NoiseMap, nCoord + vec2(-Time.x * 0.04, -Time.x * 0.03)).x;
    height += normal2 * MidWaves.x;
#endif
#if MAX_WATER_OCTAVES > 2
    nCoord = vWorldPosition.xz * WaveScale * 0.5 + WindDirection * Time.x * WindSpeed * 0.09;
    mediump float normal3 = texture2D(NoiseMap, nCoord + vec2(Time.x * 0.03, Time.x * 0.04)).x;
    height += normal3 * MidWaves.y;
#endif
#if MAX_WATER_OCTAVES > 3
    nCoord = vWorldPosition.xz * WaveScale * 1.0 + WindDirection * Time.x * WindSpeed * 0.4;
    mediump float normal4 = texture2D(NoiseMap, nCoord + vec2(-Time.x * 0.02, Time.x * 0.1)).x;
    height += normal4 * SmallWaves.x;
#endif
#if MAX_WATER_OCTAVES > 4
    nCoord = vWorldPosition.xz * WaveScale * 2.0 + WindDirection * Time.x * WindSpeed * 0.7;
    mediump float normal5 = texture2D(NoiseMap, nCoord + vec2(Time.x * 0.1, -Time.x * 0.06)).x;
    height += normal5 * BigWaves.y;
#endif

    position.y += height;

    gl_Position = mul(WorldViewProjMatrix, position);

    vScreenPosition = gl_Position;
    vPrevScreenPosition = mul(WorldViewProjPrev, position);
}

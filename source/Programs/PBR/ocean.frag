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


#define USE_MRT
#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"
precision highp float;


in highp vec3 vPosition;
in vec4 vScreenPosition;
in vec4 vPrevScreenPosition;

uniform sampler2D NormapMap;
uniform highp vec3 CameraPosition;
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform vec4 Time;
uniform float FrameTime;
uniform vec4 WorldSpaceLightPos0;
uniform vec4 LightColor0;
uniform vec2 BigWaves;
uniform vec2 MidWaves;
uniform vec2 SmallWaves;
uniform float Visibility;
uniform vec2 WindDirection;
uniform float WindSpeed;
uniform float WaveScale;
uniform float ScatterAmount;
uniform vec3 ScatterColor;
uniform float ReflDistortionAmount;
uniform float RefrDistortionAmount;
uniform float AberrationAmount;
uniform vec3 WaterExtinction;
uniform vec3 SunTransmittance;
uniform float SunFade;
uniform float ScatterFade;


//----------------------------------------------------------------------------------------------------------------------
float FresnelDielectric(const vec3 Incoming, const vec3 Normal, const float eta)
{
    // compute fresnel reflectance without explicitly computing
    // the refracted direction
    float c = abs(dot(Incoming, Normal));
    float g = eta * eta - 1.0 + c * c;

    if (g > 0.0)
    {
        g = sqrt(g);
        float A = (g - c) / (g + c);
        float B = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);
        
        return 0.5 * A * A * (1.0 + B * B);
    }

    return 1.0; // TIR (no refracted component)
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    bool aboveWater = CameraPosition.y > 0.0;

    float normalFade = 1 - min(exp(-vScreenPosition.w / 40.0), 1.0);

    vec2 nCoord = vPosition.xz * WaveScale * 0.04 + WindDirection * Time.x * WindSpeed * 0.04;
    vec3 normal0 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.015, -Time.x * 0.005)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.1 + WindDirection * Time.x * WindSpeed * 0.08;
    vec3 normal1 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.020, Time.x * 0.015)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 0.25 + WindDirection * Time.x * WindSpeed * 0.07;
    vec3 normal2 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.04, -Time.x * 0.03)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.5 + WindDirection * Time.x * WindSpeed * 0.09;
    vec3 normal3 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.03, Time.x * 0.04)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 1.0 + WindDirection * Time.x * WindSpeed * 0.4;
    vec3 normal4 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.02, Time.x * 0.1)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 2.0 + WindDirection * Time.x * WindSpeed * 0.7;
    vec3 normal5 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.1, -Time.x * 0.06)).xyz - 1.0;

    vec3 normal = normalize(normal0 * BigWaves.x + normal1 * BigWaves.y +
                            normal2 * MidWaves.x + normal3 * MidWaves.y +
                            normal4 * SmallWaves.x + normal5 * SmallWaves.y);

    vec3 nVec = mix(normal.xzy, vec3(0.0, 1.0, 0.0), normalFade); // converting normals to tangent space 
    vec3 vVec = normalize(CameraPosition - vPosition);
    vec3 lVec = WorldSpaceLightPos0.xyz;

    // normal for light scattering
    vec3 lNormal = normalize(normal0 * BigWaves.x * 0.5 + normal1 * BigWaves.y * 0.5 +
                            normal2 * MidWaves.x * 0.1 + normal3 * MidWaves.y * 0.1 +
                            normal4 * SmallWaves.x * 0.1 + normal5 * SmallWaves.y * 0.1);
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    vec3 lR = reflect(-lVec, lNormal);

    float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-WorldSpaceLightPos0.y));
    vec3 scatterColor = mix(ScatterColor * vec3(1.0, 0.4, 0.0), ScatterColor, SunTransmittance);

    // fresnel term
    float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    float fresnel = FresnelDielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
    vec3 reflection = vec3(1.0);

    const vec3 luminosity = vec3(0.30, 0.59, 0.11);
    float reflectivity = pow(dot(luminosity, reflection.rgb * 2.0), 3.0);

    vec3 R = reflect(vVec, nVec);

    float specular = min(pow(atan(max(dot(R, -lVec), 0.0) * 1.55), 1000.0) * reflectivity * 8.0, 50.0);

    vec2 rcoord = reflect(vVec, nVec).xz;
    vec2 refrOffset = nVec.xz * RefrDistortionAmount;

    // depth of potential refracted fragment
    float surfaceDepth = vScreenPosition.w;

    vec3 refraction = vec3(0.0);

    float waterSunGradient = dot(vVec, -WorldSpaceLightPos0.xyz);
    waterSunGradient = saturate(pow(waterSunGradient * 0.7 + 0.3, 2.0));  
    vec3 waterSunColor = vec3(0.0, 1.0, 0.85) * waterSunGradient;
    waterSunColor *= aboveWater ? 0.25 : 0.5;

    float waterGradient = dot(vVec, vec3(0.0, -1.0, 0.0));
    waterGradient = clamp((waterGradient * 0.5 + 0.5), 0.2, 1.0);
    vec3 watercolor = (vec3(0.0078, 0.5176, 0.700) + waterSunColor) * waterGradient * 1.5;

    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

    float fog = aboveWater ? 1.0 : surfaceDepth / Visibility;

    float darkness = Visibility * 2.0;
    darkness = saturate((CameraPosition.y + darkness) / darkness);

    refraction = mix(refraction, scatterColor, lightScatter);

    vec3 color = vec3(0.0);

    if (aboveWater)
    {
        color = mix(refraction, reflection, fresnel * 0.6);
    }
    // scatter and extinction between surface and camera
    else
    {
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));
    }

    FragData[0] = vec4(vec3(color + (LightColor0.xyz * specular)), 1.0);
    float vDepth = vScreenPosition.z;
    FragData[1].r = (vDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[2].xyz = lNormal;

    vec2 a = (vScreenPosition.xz / vScreenPosition.w);
    vec2 b = (vPrevScreenPosition.xz / vPrevScreenPosition.w);
    vec2 velocity = ((0.5 * 0.0166667) / FrameTime) * (b - a);
    FragData[3].xy = velocity;
    FragData[4].rg = vec2(fresnel, 0.0);
}

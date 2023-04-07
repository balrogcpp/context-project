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


in highp vec3 vPosition;
in highp vec4 vScreenPosition;
in highp vec4 vPrevScreenPosition;

uniform sampler2D NormapMap;
uniform highp vec3 CameraPosition;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;
uniform mediump vec4 Time;
uniform mediump float FrameTime;
uniform mediump vec4 WorldSpaceLightPos0;
uniform mediump vec4 LightColor0;
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


//----------------------------------------------------------------------------------------------------------------------
mediump float fresnelDielectric(const mediump vec3 incoming, const mediump vec3 normal, const mediump float eta)
{
    // compute fresnel reflectance without explicitly computing
    // the refracted direction
    mediump float c = abs(dot(incoming, normal));
    mediump float g = eta * eta - 1.0 + c * c;

    if (g > 0.0) {
        g = sqrt(g);
        mediump float A = (g - c) / (g + c);
        mediump float B = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);
        
        return 0.5 * A * A * (1.0 + B * B);
    }

    return 1.0; // TIR (no refracted component)
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    bool aboveWater = CameraPosition.y > 0.0;

    mediump float normalFade = 1.0 - min(exp(-vScreenPosition.w / 40.0), 1.0);

    mediump vec2 nCoord = vPosition.xz * WaveScale * 0.04 + WindDirection * Time.x * WindSpeed * 0.04;
    mediump vec3 normal0 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.015, -Time.x * 0.005)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.1 + WindDirection * Time.x * WindSpeed * 0.08;
    mediump vec3 normal1 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.020, Time.x * 0.015)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 0.25 + WindDirection * Time.x * WindSpeed * 0.07;
    mediump vec3 normal2 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.04, -Time.x * 0.03)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.5 + WindDirection * Time.x * WindSpeed * 0.09;
    mediump vec3 normal3 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.03, Time.x * 0.04)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 1.0 + WindDirection * Time.x * WindSpeed * 0.4;
    mediump vec3 normal4 = 2.0 * texture2D(NormapMap, nCoord + vec2(-Time.x * 0.02, Time.x * 0.1)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 2.0 + WindDirection * Time.x * WindSpeed * 0.7;
    mediump vec3 normal5 = 2.0 * texture2D(NormapMap, nCoord + vec2(Time.x * 0.1, -Time.x * 0.06)).xyz - 1.0;

    highp vec3 normal = normalize(normal0 * BigWaves.x + normal1 * BigWaves.y +
                            normal2 * MidWaves.x + normal3 * MidWaves.y +
                            normal4 * SmallWaves.x + normal5 * SmallWaves.y);

    highp vec3 nVec = mix(normal.xzy, vec3(0.0, 1.0, 0.0), normalFade); // converting normals to tangent space 
    highp vec3 vVec = normalize(CameraPosition - vPosition);
    highp vec3 lVec = WorldSpaceLightPos0.xyz;

    // normal for light scattering
    highp vec3 lNormal = normalize(normal0 * BigWaves.x * 0.5 + normal1 * BigWaves.y * 0.5 +
                            normal2 * MidWaves.x * 0.1 + normal3 * MidWaves.y * 0.1 +
                            normal4 * SmallWaves.x * 0.1 + normal5 * SmallWaves.y * 0.1);
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    highp vec3 lR = reflect(-lVec, lNormal);

    highp float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    highp float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-WorldSpaceLightPos0.y));
    mediump vec3 scatterColor = mix(ScatterColor * vec3(1.0, 0.4, 0.0), ScatterColor, SunTransmittance);

    // fresnel term
    mediump float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    mediump float fresnel = fresnelDielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
    vec3 reflection = vec3(1.0);

    const vec3 luminosity = vec3(0.30, 0.59, 0.11);
    float reflectivity = pow(dot(luminosity, reflection.rgb * 2.0), 3.0);

    vec3 R = reflect(vVec, nVec);

    float specular = min(pow(atan(max(dot(R, -lVec), 0.0) * 1.55), 1000.0) * reflectivity * 8.0, 50.0);

    mediump vec2 rcoord = reflect(vVec, nVec).xz;
    mediump vec2 refrOffset = nVec.xz * RefrDistortionAmount;

    // depth of potential refracted fragment
    highp float surfaceDepth = vScreenPosition.w;

    highp vec3 refraction = vec3(0.0);

    highp float waterSunGradient = dot(vVec, -WorldSpaceLightPos0.xyz);
    waterSunGradient = saturate(pow(waterSunGradient * 0.7 + 0.3, 2.0));  
    mediump vec3 waterSunColor = vec3(0.0, 1.0, 0.85) * waterSunGradient;
    waterSunColor *= aboveWater ? 0.25 : 0.5;

    mediump float waterGradient = dot(vVec, vec3(0.0, -1.0, 0.0));
    waterGradient = clamp((waterGradient * 0.5 + 0.5), 0.2, 1.0);
    mediump vec3 watercolor = (vec3(0.0078, 0.5176, 0.700) + waterSunColor) * waterGradient * 1.5;

    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

    mediump float fog = aboveWater ? 1.0 : surfaceDepth / Visibility;

    mediump float darkness = Visibility * 2.0;
    darkness = saturate((CameraPosition.y + darkness) / darkness);

    refraction = mix(refraction, scatterColor, lightScatter);

    mediump vec3 color = vec3(0.0);

    if (aboveWater) {
        color = mix(refraction, reflection, fresnel * 0.6);
    } else {
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));
    }

    FragData[0] = vec4(vec3(color + (LightColor0.xyz * specular)), 1.0);
    FragData[1].r = (vScreenPosition.z - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[2].xyz = lNormal;

    mediump vec2 a = (vScreenPosition.xz / vScreenPosition.w);
    mediump vec2 b = (vPrevScreenPosition.xz / vPrevScreenPosition.w);
    mediump vec2 velocity = ((0.5 * 0.0166667) / FrameTime) * (b - a);
    FragData[3].xy = velocity;
    FragData[4].rg = vec2(fresnel, 0.0);
}

// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"
#ifndef MAX_WATER_OCTAVES
    #define MAX_WATER_OCTAVES 2
#endif

uniform sampler2D NormapTex;
uniform sampler2D ReflectionTex;
uniform sampler2D RefractionTex;
uniform sampler2D CameraDepthTex;
uniform highp vec3 CameraPosition;
uniform highp mat4 ViewMatrix;
uniform vec4 Time;
uniform vec4 FogColour;
uniform vec4 FogParams;
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

float fresnelDielectric(const vec3 incoming, const vec3 normal, float eta)
{
    // compute fresnel reflectance without explicitly computing
    // the refracted direction
    float c = abs(dot(incoming, normal));
    float g = eta * eta - 1.0 + c * c;

    if (g > 0.0) {
        g = sqrt(g);
        float a = (g - c) / (g + c);
        float b = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);
        
        return 0.5 * a * a * (1.0 + b * b);
    } else {
        return 1.0; // TIR (no refracted component)
    }    
}

in highp vec3 vWorldPosition;
in vec4 vProjectionCoord;
void main()
{
    bool aboveWater = CameraPosition.y > vWorldPosition.y;
    float surfaceDepth = gl_FragCoord.z / gl_FragCoord.w;
    float normalFade = 1.0 - min(exp(-surfaceDepth / 40.0), 1.0);

    vec2 nCoord = vWorldPosition.xz * WaveScale * 0.04 + WindDirection * Time.x * WindSpeed * 0.04;
    highp vec3 normal0 = 2.0 * texture2D(NormapTex, nCoord + vec2(-Time.x * 0.015, -Time.x * 0.005)).xyz - 1.0;
    highp vec3 normal = normal0 * BigWaves.x;
    highp vec3 lNormal = normal0 * BigWaves.x * 0.5;

#if MAX_WATER_OCTAVES > 0
    nCoord = vWorldPosition.xz * WaveScale * 0.1 + WindDirection * Time.x * WindSpeed * 0.08;
    highp vec3 normal1 = 2.0 * texture2D(NormapTex, nCoord + vec2(Time.x * 0.020, Time.x * 0.015)).xyz - 1.0;
    normal += normal1 * BigWaves.y;
    lNormal += normal1 * BigWaves.y * 0.5;
#endif
#if MAX_WATER_OCTAVES > 1
    nCoord = vWorldPosition.xz * WaveScale * 0.25 + WindDirection * Time.x * WindSpeed * 0.07;
    highp vec3 normal2 = 2.0 * texture2D(NormapTex, nCoord + vec2(-Time.x * 0.04, -Time.x * 0.03)).xyz - 1.0;
    normal += normal2 * MidWaves.x;
    lNormal += normal2 * MidWaves.x * 0.1;
#endif
#if MAX_WATER_OCTAVES > 2
    nCoord = vWorldPosition.xz * WaveScale * 0.5 + WindDirection * Time.x * WindSpeed * 0.09;
    highp vec3 normal3 = 2.0 * texture2D(NormapTex, nCoord + vec2(Time.x * 0.03, Time.x * 0.04)).xyz - 1.0;
    normal += normal3 * MidWaves.y;
    lNormal += normal3 * MidWaves.y * 0.1;
#endif
#if MAX_WATER_OCTAVES > 3
    nCoord = vWorldPosition.xz * WaveScale * 1.0 + WindDirection * Time.x * WindSpeed * 0.4;
    highp vec3 normal4 = 2.0 * texture2D(NormapTex, nCoord + vec2(-Time.x * 0.02, Time.x * 0.1)).xyz - 1.0;
    normal += normal4 * SmallWaves.x;
    lNormal += normal4 * SmallWaves.x * 0.1;
#endif
#if MAX_WATER_OCTAVES > 4
    nCoord = vWorldPosition.xz * WaveScale * 2.0 + WindDirection * Time.x * WindSpeed * 0.7;
    highp vec3 normal5 = 2.0 * texture2D(NormapTex, nCoord + vec2(Time.x * 0.1, -Time.x * 0.06)).xyz - 1.0;
    normal += normal5 * SmallWaves.y;
    lNormal += normal5 * SmallWaves.y * 0.1;
#endif

    normal = normalize(normal);

    highp vec3 nVec = mix(normal.xzy, vec3(0.0, 1.0, 0.0), normalFade); // converting normals to tangent space
    highp vec3 vVec = normalize(CameraPosition - vWorldPosition);
    highp vec3 lVec = WorldSpaceLightPos0.xyz;

    // normal for light scattering
    lNormal = normalize(lNormal);
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    highp vec3 lR = reflect(-lVec, lNormal);

    highp float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    highp float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-WorldSpaceLightPos0.y));
    vec3 scatterColor = mix(ScatterColor * vec3(1.0, 0.4, 0.0), ScatterColor, SunTransmittance);

    // fresnel term
    float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    float fresnel = fresnelDielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
    vec2 fragCoord = vProjectionCoord.xy / vProjectionCoord.w;
    fragCoord = clamp(fragCoord, 0.002, 0.998);
    vec3 reflection = texture2D(ReflectionTex, fragCoord + nVec.xz * vec2(ReflDistortionAmount, ReflDistortionAmount)).rgb;
#ifdef FORCE_TONEMAP
    reflection = SRGBtoLINEAR(reflection);
#endif

    const vec3 luminosity = vec3(0.30, 0.59, 0.11);
    float reflectivity = pow(dot(luminosity, reflection.rgb * 2.0), 3.0);

    highp vec3 R = reflect(vVec, nVec);

    float specular = min(pow(atan(max(dot(R, -lVec), 0.0) * 1.55), 1000.0) * reflectivity * 8.0, 50.0);

    vec2 rcoord = reflect(vVec, nVec).xz;
    vec2 refrOffset = nVec.xz * RefrDistortionAmount;

    // depth of potential refracted fragment
    float refractedDepth = (texture2D(CameraDepthTex, fragCoord - refrOffset * 2.0).x) * FarClipDistance;

    float distortFade = saturate((refractedDepth - surfaceDepth) * 4.0);

//    highp vec3 refraction;
//    refraction.r = texture2D(RefractionTex, fragCoord - (refrOffset - rcoord * -AberrationAmount) * distortFade).r;
//    refraction.g = texture2D(RefractionTex, fragCoord - refrOffset * distortFade).g;
//    refraction.b = texture2D(RefractionTex, fragCoord - (refrOffset - rcoord * AberrationAmount) * distortFade).b;
    vec3 refraction = texture2D(RefractionTex, fragCoord - refrOffset * distortFade).rgb;
#ifdef FORCE_TONEMAP
    refraction = SRGBtoLINEAR(refraction);
#endif

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

    vec3 color = vec3(0.0, 0.0, 0.0);

    if (aboveWater) {
        color = mix(refraction, reflection, fresnel * 0.6);
    } else {
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));
    }

    color += LightColor0.xyz * specular;
    color = ApplyFog(color, FogParams.x, FogColour.rgb, gl_FragCoord.z / gl_FragCoord.w, vVec, WorldSpaceLightPos0.xyz, CameraPosition);

    EvaluateBuffer(color);
#ifdef HAS_MRT
    FragData[MRT_NORMALS].xyz = normalize(mul(ViewMatrix, vec4(lNormal, 0.0))).xyz;
#endif
}

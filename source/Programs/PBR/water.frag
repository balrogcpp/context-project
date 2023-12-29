// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"

uniform sampler2D NormalTex;
uniform sampler2D RefractionTex;
uniform sampler2D CameraDepthTex;
//uniform sampler2D ReflectionTex;
uniform samplerCube SpecularEnvTex;
uniform highp vec3 CameraPosition;
uniform highp mat4 ViewMatrix;
uniform float Time;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform vec4 LightDir0;
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
uniform vec3 SunExtinction;

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

vec3 intercept(const vec3 lineP, const vec3 lineN, const vec3 planeN, float planeD)
{
    float distance = (planeD - dot(planeN, lineP)) / dot(lineN, planeN);
    return lineP + lineN * distance;
}

vec3 perturb(sampler2D tex, const vec2 coords, float bend)
{
    vec3 col = vec3(0.0, 0.0, 0.0);

    vec2 windDir = WindDirection;
    float windSpeed = WindSpeed;
    float scale = WaveScale;

    // might need to swizzle, not sure
    vec2 nCoord = coords * (scale * 0.04) + windDir * Time * (windSpeed * 0.03);
    col += texture2D(tex, nCoord + vec2(-Time * 0.005, -Time * 0.01)).rgb * 0.20;

    nCoord = coords * (scale * 0.1) + windDir * Time * (windSpeed * 0.05) - (col.xy / col.z) * bend;
    col += texture2D(tex, nCoord + vec2(+Time * 0.01, +Time * 0.005)).rgb * 0.20;

    nCoord = coords * (scale * 0.25) + windDir * Time * (windSpeed * 0.1) - (col.xy / col.z) * bend;
    col += texture2D(tex, nCoord + vec2(-Time * 0.02, -Time * 0.03)).rgb * 0.20;

    nCoord = coords * (scale * 0.5) + windDir * Time * (windSpeed * 0.2) - (col.xy / col.z) * bend;
    col += texture2D(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    nCoord = coords * (scale * 1.0) + windDir * Time * (windSpeed * 1.0) - (col.xy / col.z) * bend;
    col += texture2D(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    nCoord = coords * (scale * 2.0) + windDir * Time * (windSpeed * 1.3) - (col.xy / col.z) * bend;
    col += texture2D(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    return col;
}

in highp vec3 vPosition;
in highp vec4 vProjectionCoord;
void main()
{
    vec3 SunTransmittance = max(1.0 - exp(LightDir0.y * SunExtinction), 0.0);
    float SunFade = clamp((0.1 - LightDir0.y) * 10.0, 0.0, 1.0);
    float ScatterFade = clamp((0.15 - LightDir0.y) * 4.0, 0.0, 1.0);

    vec2 fragCoord = vProjectionCoord.xy / vProjectionCoord.w;
    fragCoord = clamp(fragCoord, 0.002, 0.998);

    //bool aboveWater = CameraPosition.y > vWorldPosition.y;
    const bool aboveWater = true;

    highp float surfaceDepth = gl_FragCoord.z / gl_FragCoord.w;
    float normalFade = 1.0 - min(exp(-surfaceDepth / 40.0), 1.0);

    vec2 nCoord = vPosition.xz * WaveScale * 0.04 + WindDirection * Time * WindSpeed * 0.04;
    vec3 normal0 = 2.0 * texture2D(NormalTex, nCoord + vec2(-Time * 0.015, -Time * 0.005)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.1 + WindDirection * Time * WindSpeed * 0.08;
    vec3 normal1 = 2.0 * texture2D(NormalTex, nCoord + vec2(Time * 0.020, Time * 0.015)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 0.25 + WindDirection * Time * WindSpeed * 0.07;
    vec3 normal2 = 2.0 * texture2D(NormalTex, nCoord + vec2(-Time * 0.04, -Time * 0.03)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.5 + WindDirection * Time * WindSpeed * 0.09;
    vec3 normal3 = 2.0 * texture2D(NormalTex, nCoord + vec2(Time * 0.03, Time * 0.04)).xyz - 1.0;

//    nCoord = vWorldPosition.xz * WaveScale * 1.0 + WindDirection * Time * WindSpeed * 0.4;
//    vec3 normal4 = 2.0 * texture2D(NormalTex, nCoord + vec2(-Time * 0.02, Time * 0.1)).xyz - 1.0;
//    nCoord = vWorldPosition.xz * WaveScale * 2.0 + WindDirection * Time * WindSpeed * 0.7;
//    vec3 normal5 = 2.0 * texture2D(NormalTex, nCoord + vec2(Time * 0.1, -Time * 0.06)).xyz - 1.0;

    vec3 normal = normalize(normal0 * BigWaves.x + normal1 * BigWaves.y
    + normal2 * MidWaves.x + normal3 * MidWaves.y
//    + normal4 * SmallWaves.x + normal5 * SmallWaves.y
    );

    highp vec3 nVec = mix(normal.xzy, vec3(0.0, 1.0, 0.0), normalFade); // converting normals to tangent space
    highp vec3 vVec = normalize(CameraPosition - vPosition);
    highp vec3 lVec = LightDir0.xyz;

    // normal for light scattering
    highp vec3 lNormal = normalize(normal0 * BigWaves.x * 0.5 + normal1 * BigWaves.y * 0.5
                                   + normal2 * MidWaves.x * 0.1 + normal3 * MidWaves.y * 0.1
//                                   + normal4 * SmallWaves.x * 0.1 + normal5 * SmallWaves.y * 0.1
    );
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    highp vec3 lR = reflect(-lVec, lNormal);

    float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-LightDir0.y));
    vec3 scatterColor = mix((ScatterColor * vec3(1.0, 0.4, 0.0))/10.0, (ScatterColor), SunTransmittance);

    // fresnel term
    float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    float fresnel = fresnelDielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
//    vec3 reflection = texture2D(ReflectionTex, fragCoord + nVec.xz * vec2(ReflDistortionAmount, ReflDistortionAmount * 6.0)).rgb;
    vec3 R1 = -normalize(reflect(vVec, nVec));
    vec3 reflection = textureCube(SpecularEnvTex, vec3(-R1.x, R1.y, R1.z)).rgb;
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
    float refractedDepth = texture2D(CameraDepthTex, fragCoord - refrOffset * 0.9).x * (FarClipDistance - NearClipDistance) + NearClipDistance;

    float distortFade = saturate((refractedDepth - surfaceDepth) * 4.0);
    refractedDepth = texture2D(CameraDepthTex, fragCoord - refrOffset * distortFade).x * (FarClipDistance - NearClipDistance) + NearClipDistance;

    vec3 refraction;
//#ifdef GL_ES
    refraction.rgb = texture2D(RefractionTex, fragCoord - refrOffset * distortFade).rgb;
//#else
//    refraction.r = texture2D(RefractionTex, fragCoord - (refrOffset - rcoord * -AberrationAmount) * distortFade).r;
//    refraction.g = texture2D(RefractionTex, fragCoord - refrOffset * distortFade).g;
//    refraction.b = texture2D(RefractionTex, fragCoord - (refrOffset - rcoord * AberrationAmount) * distortFade).b;
//#endif
#ifdef FORCE_TONEMAP
    refraction = SRGBtoLINEAR(refraction);
#endif

    // caustics
    float causticdepth = refractedDepth - surfaceDepth;
    causticdepth = 1.0 - saturate(causticdepth / Visibility);
    causticdepth = saturate(causticdepth);
    float causticR = 1.0 - perturb(NormalTex, vPosition.xz, causticdepth).z;

    float caustics = saturate(pow(causticR * 5.5, 5.5 * causticdepth));// * NdotL * SunFade * causticdepth;

    float waterSunGradient = dot(vVec, -LightDir0.xyz);
    waterSunGradient = saturate(pow(waterSunGradient * 0.7 + 0.3, 2.0));
    vec3 waterSunColor = SRGBtoLINEAR(vec3(0.0, 1.0, 0.85) * waterSunGradient);
    waterSunColor *= aboveWater ? 0.25 : 0.5;

    float waterGradient = dot(vVec, vec3(0.0, -1.0, 0.0));
    waterGradient = clamp((waterGradient * 0.5 + 0.5), 0.2, 1.0);
    vec3 watercolor = ((vec3(0.0078, 0.5176, 0.700) + waterSunColor)/10.0) * waterGradient * 1.5;

    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

//    float fog = aboveWater ? 1.0 : surfaceDepth / Visibility;
    float fog = aboveWater ? (refractedDepth - surfaceDepth) / Visibility : surfaceDepth / Visibility;

    float darkness = Visibility * 2.0;
    darkness = saturate((CameraPosition.y + darkness) / darkness);

    refraction = mix(refraction, scatterColor, lightScatter);
    refraction = mix(refraction, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));

    vec3 color;

    if (aboveWater) {
        color = mix(refraction, reflection, fresnel * 0.6);
    } else {
        // scatter and extinction between surface and camera
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));
    }

    color += LightColor0.xyz * specular;
    color = ApplyFog(color, FogParams.x, FogColour.rgb, surfaceDepth, vVec, LightDir0.xyz, CameraPosition);

    EvaluateBuffer(color);
#ifdef HAS_MRT
    FragData[MRT_NORMALS].xyz = normalize(mul(ViewMatrix, vec4(lNormal, 0.0))).xyz;
    //FragData[MRT_GLOSS].rgb = vec3(0.1, 0.2, 1.0);
#endif
}

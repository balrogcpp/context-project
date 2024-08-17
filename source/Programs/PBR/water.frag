// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "math.glsl"
#include "fog.glsl"
#include "srgb.glsl"

uniform sampler2D ReflectionTex;
uniform sampler2D RefractionTex;
uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D CausticTex;
uniform sampler2D FoamTex;

uniform highp vec3 CameraPosition;
uniform highp mat4 ViewMatrix;
uniform vec4 ViewportSize;
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform float Time;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform vec3 LightDir0;
uniform vec3 LightColor0;
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

float FresnelDielectric(const vec3 incoming, const vec3 normal, float eta)
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

vec3 perturb(const sampler2D tex, const vec2 coords, float bend)
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
in highp vec2 vUV0;
void main()
{
    vec3 SunTransmittance = max(1.0 - exp(LightDir0.y * SunExtinction), 0.0);
    float SunFade = clamp((0.1 - LightDir0.y) * 10.0, 0.0, 1.0);
    float ScatterFade = clamp((0.15 - LightDir0.y) * 4.0, 0.0, 1.0);

    vec2 fragCoord = gl_FragCoord.xy * ViewportSize.zw;
    fragCoord = clamp(fragCoord, 0.002, 0.998);

    bool aboveWater = gl_FrontFacing;

    float fragDepth = gl_FragCoord.z / gl_FragCoord.w;
    float normalFade = 1.0 - min(exp(-fragDepth / 40.0), 1.0);

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
    highp vec3 lVec = -LightDir0.xyz;

    // normal for light scattering
    highp vec3 lNormal = normalize(normal0 * BigWaves.x * 0.5 + normal1 * BigWaves.y * 0.5
                                   + normal2 * MidWaves.x * 0.1 + normal3 * MidWaves.y * 0.1
//                                   + normal4 * SmallWaves.x * 0.1 + normal5 * SmallWaves.y * 0.1
    );
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    highp vec3 lR = reflect(-lVec, lNormal);

    float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-LightDir0.y));
    vec3 scatterColor = mix((ScatterColor * vec3(1.0, 0.4, 0.0)), (ScatterColor), SunTransmittance);

    // fresnel term
    float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    float fresnel = FresnelDielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
    vec3 reflection = textureLod(ReflectionTex, fragCoord + nVec.xz * vec2(ReflDistortionAmount, ReflDistortionAmount * 6.0), 0.0).rgb;
#ifdef FORCE_TONEMAP
    reflection = Inverse_Tonemap_Unreal(reflection);
#endif

    const vec3 luminosity = vec3(0.30, 0.59, 0.11);
    float reflectivity = pow(dot(luminosity, reflection.rgb * 2.0), 3.0);

    highp vec3 R = reflect(vVec, nVec);

    float specular = min(pow(atan(max(dot(R, -lVec), 0.0) * 1.55), 1000.0) * reflectivity * 8.0, 50.0);

    vec2 rcoord = reflect(vVec, nVec).xz;
    vec2 refrOffset = nVec.xz * RefrDistortionAmount;

    // depth of potential refracted fragment
    float refractedDepth = textureLod(DepthTex, fragCoord - refrOffset * 0.9, 0.0).x * (FarClipDistance - NearClipDistance) + NearClipDistance;
    highp float surfaceDepth = fragDepth;

    float distortFade = saturate((refractedDepth - surfaceDepth) * 4.0);

#ifndef GL_ES
    vec3 refraction;
    refraction.r = textureLod(RefractionTex, fragCoord - (refrOffset - rcoord * -AberrationAmount) * distortFade, 0.0).r;
    refraction.g = textureLod(RefractionTex, fragCoord - refrOffset * distortFade, 0.0).g;
    refraction.b = textureLod(RefractionTex, fragCoord - (refrOffset - rcoord * AberrationAmount) * distortFade, 0.0).b;
#else
    vec3 refraction = textureLod(RefractionTex, fragCoord - refrOffset * distortFade, 0.0).rgb;
#endif
#ifdef FORCE_TONEMAP
    refraction = Inverse_Tonemap_Unreal(refraction);
#endif

    float waterSunGradient = dot(vVec, LightDir0.xyz);
    waterSunGradient = saturate(pow(waterSunGradient * 0.7 + 0.3, 2.0));
    vec3 waterSunColor = (vec3(0.0, 1.0, 0.85) * waterSunGradient);
    waterSunColor *= aboveWater ? 0.25 : 0.5;

    float waterGradient = dot(vVec, vec3(0.0, -1.0, 0.0));
    waterGradient = clamp((waterGradient * 0.5 + 0.5), 0.2, 1.0);
    vec3 watercolor = (vec3(0.0078, 0.5176, 0.700) + waterSunColor) * waterGradient * 1.5;

    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

    float fog1 = aboveWater ? saturate((refractedDepth - surfaceDepth) / Visibility) : surfaceDepth / Visibility;

    float darkness = Visibility * 2.0;
    darkness = saturate((CameraPosition.y + darkness) / darkness);
    refraction = mix(refraction, scatterColor, lightScatter);

    // underwater
    vec3 waterColor = (vec3(0.0078, 0.5176, 0.700) + waterSunColor) * waterGradient * 1.5;
    vec3 waterEyePos = vPosition;
    vec3 diffuse = refraction;
    float NdotL = max(dot(vec3(0.0, 1.0, 0.0), -LightDir0), 0.0); //float NdotL = max(dot(normal, -LightDir0), 0.0);
    vec3 sunLight = LightColor0.rgb * NdotL * SunFade;

    // sky illumination
    float skyBright = max(dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5, 0.0);
    vec3 skyLight = mix(vec3(1.0, 0.5, 0.0) * 0.05, vec3(0.2, 0.5, 1.0) * 1.5, SunTransmittance);
    skyLight *= skyBright;

    // ground illumination
    float groundBright = max(dot(normal, vec3(0.0, -1.0, 0.0)) * 0.5 + 0.5, 0.0);
    float sunLerp = saturate(1.0 - exp(LightDir0.y));
    vec3 groundLight = 0.3 * vec3(sunLerp, sunLerp, sunLerp);
    groundLight *= groundBright;

    vec3 EV = vVec;
    float underwaterFresnel = pow(saturate(1.0 - dot(normal, EV)), 2.0) * sunLerp;

    // water color
    float topfog = (refractedDepth - surfaceDepth) / Visibility;
    topfog = saturate(topfog);

    float viewDepth = refractedDepth - surfaceDepth;

    float underfog = viewDepth / Visibility;
    underfog = saturate(underfog);

    float depth = refractedDepth - surfaceDepth; // water depth

    float far = viewDepth / 1000.0;
    float shorecut = aboveWater ? smoothstep(-0.001, 0.001, depth) : smoothstep(-5.0 * max(far, 0.0001), -4.0 * max(far, 0.0001), depth);
    float shorewetcut = smoothstep(-0.18, -0.000, depth + 0.01);

    depth /= Visibility;
    depth = saturate(depth);

    float fog = aboveWater ? topfog : underfog;
    fog *= shorecut;

//    float darkness = Visibility * 1.5;
//    darkness = mix(1.0, saturate((CameraPosition.y + darkness) / darkness), shorecut);

    float fogdarkness = Visibility * 2.0;
    fogdarkness = mix(1.0, saturate((CameraPosition.y + fogdarkness) / fogdarkness), shorecut) * ScatterFade;

    // caustics
    float causticdepth = refractedDepth - surfaceDepth; // caustic depth
    causticdepth = 1.0 - saturate(causticdepth / Visibility);
    causticdepth = saturate(causticdepth);

//    vec3 normalMap = perturb(CausticTex, causticPos.xz, causticdepth) * 2.0 - 1.0;
//    normalMap = normalMap.xzy;
//    normalMap.xz *= -1;
//    vec3 causticnorm = normalMap;

//    float fresnel = pow(saturate(dot(_WorldSpaceLightPos0.xyz, causticnorm)), 2.0);

    float causticR = 1.0 - perturb(CausticTex, vPosition.xz, causticdepth).z;

    float caustics = saturate(pow(causticR * 5.5, 5.5 * causticdepth)) * NdotL * SunFade * causticdepth;

    // not yet implemented
    //if(causticFringe)
    //{
    //    float causticG = 1.0-perturb(NormalSampler,causticPos.st+(1.0-causticdepth)*aberration,causticdepth).z;
    //    float causticB = 1.0-perturb(NormalSampler,causticPos.st+(1.0-causticdepth)*aberration*2.0,causticdepth).z;
    //    caustics = clamp(pow(vec3(causticR,causticG,causticB)*5.5,vec3(5.5*causticdepth)),0.0,1.0)*NdotL*_SunFade*causticdepth;
    //}

    vec3 underwaterSunLight = saturate((sunLight + 0.9) - (1.0 - caustics)) * causticdepth + (sunLight * caustics);

    underwaterSunLight = mix(underwaterSunLight, underwaterSunLight * waterColor, saturate((1.0 - causticdepth) / WaterExtinction));
    vec3 waterPenetration = saturate(depth / WaterExtinction);
    skyLight = mix(skyLight, skyLight * waterColor, waterPenetration);
    groundLight = mix(groundLight, groundLight * waterColor, waterPenetration);

    sunLight = mix(sunLight, mix(underwaterSunLight , (waterColor * 0.8 + 0.4) * SunFade, underwaterFresnel), shorecut);

    vec3 color = vec3(sunLight + skyLight * 0.7 + groundLight * 0.8) * darkness;

    waterColor = SRGBtoLINEAR(waterColor);
    watercolor = SRGBtoLINEAR(watercolor);
    waterColor = mix(waterColor * 0.3 * SunFade, waterColor, SunTransmittance);

    //vec3 fogging = mix((refraction * 0.2 + 0.8) * mix(vec3(1.2, 0.95, 0.58) * 0.8, vec3(1.1, 0.85, 0.5) * 0.8, shorewetcut) * color, waterColor * fogdarkness, saturate(fog / WaterExtinction)); // adding water color fog
    vec3 fogging = mix(refraction * sunLight * color, waterColor * fogdarkness, saturate(fog / WaterExtinction)); // adding water color fog

    if (aboveWater) {
        color = mix(fogging, reflection, fresnel * 0.6);
    } else {
        // scatter and extinction between surface and camera
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog1 / WaterExtinction));
    }

    // foam not implementer
    // if(abs(causticdepth) < 0.1)
    //{
    //    vec2 uv_offset = Time * vec2(0.05, 0.04);
    //    float foam_noise = clamp(pow(texture2D(FoamTex, (vUV0*20.0) - uv_offset).r, 10.0)*40.0, 0.0, 0.2);
    //    float foam_mix = clamp(pow((1.0-(depth) + foam_noise), 8.0) * foam_noise * 0.4, 0.0, 1.0);
    //    color = mix(color, vec3(1.0, 1.0, 1.0), foam_mix);
    //}

    color += LightColor0 * specular;
#ifdef FORCE_FOG
    color = ApplyFog(color, FogParams.x, FogColour.rgb, surfaceDepth, vVec, LightDir0.xyz, CameraPosition);
#endif

    EvaluateBuffer(vec4(color, 1.0), (fragDepth - NearClipDistance) / (FarClipDistance - NearClipDistance), mul(mat3(ViewMatrix), lNormal), vec2(0.0, 0.0));
}

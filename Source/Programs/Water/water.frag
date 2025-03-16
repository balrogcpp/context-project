// created by Andrey Vasiliev
//? #version 400

#include "fog.glsl"
#include "tonemap.glsl"

#define saturate(x) clamp(x, 0.0, 1.0)
#define lerp mix
#define PI 3.14159265359
#define HALF_PI 1.570796327

uniform lowp sampler2D ReflectionTex;
uniform lowp sampler2D RefractionTex;
uniform mediump sampler2D DepthTex;
uniform mediump sampler3D NormalTex;
uniform lowp sampler2D CausticTex;
uniform lowp sampler2D FoamTex;

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

// max absolute error 1.3x10^-3
// Eberly's odd polynomial degree 5 - respect bounds
// 4 VGPR, 14 FR (10 FR, 1 QR), 2 scalar
// input [0, infinity] and output [0, PI/2]
float atanFastPositive(const float x) 
{ 
    float t0 = (x < 1.0) ? x : 1.0 / x;
    float t1 = t0 * t0;
    float poly = 0.0872929;
    poly = -0.301895 + poly * t1;
    poly = 1.0 + poly * t1;
    poly = poly * t0;
    return (x < 1.0) ? poly : HALF_PI - poly;
}

// 4 VGPR, 16 FR (12 FR, 1 QR), 2 scalar
// input [-infinity, infinity] and output [-PI/2, PI/2]
float atanFast(const float x)
{
    float t0 = atanFastPositive(abs(x));     
    return (x < 0.0) ? -t0: t0;
}

float sq(const float x)
{
    return x * x;
}

float pow3(const float x)
{
    return (x * x) * x;
}

vec3 intercept(const vec3 lineP, const vec3 lineN, const vec3 planeN, const float planeD)
{
    float distance = (planeD - dot(planeN, lineP)) / dot(lineN, planeN);
    return lineP + lineN * distance;
}

float fresnel_dielectric(const vec3 incoming, const vec3 normal, const float eta)
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

vec3 perturb(const sampler2D tex, const vec2 coords, const float bend)
{
    vec3 col = vec3(0.0, 0.0, 0.0);

    vec2 windDir = WindDirection;
    float windSpeed = WindSpeed;
    float scale = WaveScale;

    // might need to swizzle, not sure
    vec2 nCoord = coords * (scale * 0.04) + windDir * Time * (windSpeed * 0.03);
    col += texture(tex, nCoord + vec2(-Time * 0.005, -Time * 0.01)).rgb * 0.20;

    nCoord = coords * (scale * 0.1) + windDir * Time * (windSpeed * 0.05) - (col.xy / col.z) * bend;
    col += texture(tex, nCoord + vec2(+Time * 0.01, +Time * 0.005)).rgb * 0.20;

    nCoord = coords * (scale * 0.25) + windDir * Time * (windSpeed * 0.1) - (col.xy / col.z) * bend;
    col += texture(tex, nCoord + vec2(-Time * 0.02, -Time * 0.03)).rgb * 0.20;

    nCoord = coords * (scale * 0.5) + windDir * Time * (windSpeed * 0.2) - (col.xy / col.z) * bend;
    col += texture(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    nCoord = coords * (scale * 1.0) + windDir * Time * (windSpeed * 1.0) - (col.xy / col.z) * bend;
    col += texture(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    nCoord = coords * (scale * 2.0) + windDir * Time * (windSpeed * 1.3) - (col.xy / col.z) * bend;
    col += texture(tex, nCoord + vec2(+Time * 0.03, +Time * 0.02)).rgb * 0.15;

    return col;
}

in highp vec3 vPosition;
in highp vec2 vUV0;
out vec3 FragColor;
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
    /*
    vec2 nCoord = vPosition.xz * WaveScale * 0.04 + WindDirection * Time * WindSpeed * 0.04;
    vec3 normal0 = 2.0 * texture(vec2NormalTex, nCoord + vec2(-Time * 0.015, -Time * 0.005)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.1 + WindDirection * Time * WindSpeed * 0.08;
    vec3 normal1 = 2.0 * texture(NormalTex, nCoord + vec2(Time * 0.020, Time * 0.015)).xyz - 1.0;

    nCoord = vPosition.xz * WaveScale * 0.25 + WindDirection * Time * WindSpeed * 0.07;
    vec3 normal2 = 2.0 * texture(NormalTex, nCoord + vec2(-Time * 0.04, -Time * 0.03)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 0.5 + WindDirection * Time * WindSpeed * 0.09;
    vec3 normal3 = 2.0 * texture(NormalTex, nCoord + vec2(Time * 0.03, Time * 0.04)).xyz - 1.0;

#if 0
    nCoord = vPosition.xz * WaveScale * 1.0 + WindDirection * Time * WindSpeed * 0.4;
    vec3 normal4 = 2.0 * texture(NormalTex, nCoord + vec2(-Time * 0.02, Time * 0.1)).xyz - 1.0;
    nCoord = vPosition.xz * WaveScale * 2.0 + WindDirection * Time * WindSpeed * 0.7;
    vec3 normal5 = 2.0 * texture(NormalTex, nCoord + vec2(Time * 0.1, -Time * 0.06)).xyz - 1.0;
#endif

    vec3 normal = normalize(normal0 * BigWaves.x + normal1 * BigWaves.y
    + normal2 * MidWaves.x + normal3 * MidWaves.y
#if 0
    + normal4 * SmallWaves.x + normal5 * SmallWaves.y
#endif
    );
    */

    vec2 nCoord = vPosition.xz * WaveScale * 0.04 + WindDirection * Time * WindSpeed * 0.04;
    vec3 normal = texture(NormalTex, vec3(nCoord+ vec2(-Time * 0.015, -Time * 0.005), Time * 0.1)).rgb;
    normal = normal * 2.0 - 1.0;
    normal.z = 1.0;
    normal = normalize(normal);

    highp vec3 nVec = mix(normal.xzy, vec3(0.0, 1.0, 0.0), normalFade); // converting normals to tangent space
    highp vec3 vVec = normalize(CameraPosition - vPosition);
    highp vec3 lVec = -LightDir0.xyz;

    // normal for light scattering
/*    highp vec3 lNormal = normalize(normal0 * BigWaves.x * 0.5 + normal1 * BigWaves.y * 0.5
                                   + normal2 * MidWaves.x * 0.1 + normal3 * MidWaves.y * 0.1
#if 0
                                   + normal4 * SmallWaves.x * 0.1 + normal5 * SmallWaves.y * 0.1
#endif
    );*/
    highp vec3 lNormal = normal;
    lNormal = mix(lNormal.xzy, vec3(0.0, 1.0, 0.0), normalFade);

    highp vec3 lR = reflect(-lVec, lNormal);

    float s = max(dot(lR, vVec) * 2.0 - 1.2, 0.0);
    float lightScatter = saturate((saturate(dot(-lVec, lNormal) * 0.7 + 0.3) * s) * ScatterAmount) * SunFade * saturate(1.0 - exp(-LightDir0.y));
    vec3 scatterColor = mix((ScatterColor * vec3(1.0, 0.4, 0.0)), ScatterColor, SunTransmittance);

    // fresnel term
    float ior = aboveWater ? (1.333 / 1.0) : (1.0 / 1.333); // air to water; water to air
    float fresnel = fresnel_dielectric(-vVec, nVec, ior);

    // texture edge bleed removal is handled by clip plane offset
    vec3 reflection = textureLod(ReflectionTex, fragCoord + nVec.xz * vec2(ReflDistortionAmount, ReflDistortionAmount * 6.0), 0.0).rgb;
    reflection = inverseTonemapSRGB(reflection);

    const vec3 luminosity = vec3(0.30, 0.59, 0.11);
    float reflectivity = pow3(dot(luminosity, reflection.rgb * 2.0));

    highp vec3 R = reflect(vVec, nVec);

    float specular = min(pow(atanFastPositive(max(dot(R, -lVec), 0.0) * 1.55), 1000.0) * reflectivity * 8.0, 50.0);

    vec2 rcoord = reflect(vVec, nVec).xz;
    vec2 refrOffset = nVec.xz * RefrDistortionAmount;

    // depth of potential refracted fragment
    float refractedDepth = textureLod(DepthTex, fragCoord - refrOffset * 2.0, 0.0).x;
    highp float surfaceDepth = fragDepth;

    float distortFade = saturate((refractedDepth - surfaceDepth) * 4.0);

#if !defined(GL_ES)
    vec3 refraction;
    refraction.r = textureLod(RefractionTex, fragCoord - (refrOffset - rcoord * -AberrationAmount) * distortFade, 0.0).r;
    refraction.g = textureLod(RefractionTex, fragCoord - refrOffset * distortFade, 0.0).g;
    refraction.b = textureLod(RefractionTex, fragCoord - (refrOffset - rcoord * AberrationAmount) * distortFade, 0.0).b;
    refraction = inverseTonemapSRGB(refraction);
    refraction = mix(refraction, scatterColor, lightScatter);
#else
    vec3 refraction = textureLod(RefractionTex, fragCoord - refrOffset * distortFade, 0.0).rgb;
    refraction = inverseTonemapSRGB(refraction);
    refraction = mix(refraction, scatterColor, lightScatter);
#endif

    float waterSunGradient = dot(vVec, LightDir0.xyz);
    waterSunGradient = saturate(sq(waterSunGradient * 0.7 + 0.3));
    vec3 waterSunColor = (vec3(0.0, 1.0, 0.85) * waterSunGradient);
    waterSunColor *= aboveWater ? 0.25 : 0.5;

    float waterGradient = dot(vVec, vec3(0.0, -1.0, 0.0));
    waterGradient = clamp((waterGradient * 0.5 + 0.5), 0.2, 1.0);
    vec3 watercolor = (vec3(0.0078, 0.5176, 0.700) + waterSunColor) * waterGradient * 1.5;

//    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

//    float fog = aboveWater ? 1.0 : surfaceDepth / Visibility;

    float darkness = Visibility * 2.0;
    darkness = saturate((CameraPosition.y + darkness) / darkness);

    // water color
    float topfog = (refractedDepth - surfaceDepth) / Visibility;
    topfog = saturate(topfog);

    float viewDepth = refractedDepth - surfaceDepth;

    float underfog = surfaceDepth / Visibility;
    underfog = saturate(underfog);

    float depth = refractedDepth - surfaceDepth; // water depth

    float far = viewDepth / 1000.0;
    float shorecut = aboveWater ? smoothstep(-0.001, 0.001, depth) : smoothstep(-5.0 * max(far, 0.0001), -4.0 * max(far, 0.0001), depth);
    float shorewetcut = smoothstep(-0.18, -0.000, depth + 0.01);

    depth /= Visibility;
    depth = saturate(depth);

    float fog = aboveWater ? topfog * shorecut : underfog;

    float fogdarkness = Visibility * 2.0;
    fogdarkness = mix(1.0, saturate((CameraPosition.y + fogdarkness) / fogdarkness), shorecut) * ScatterFade;

    watercolor = mix(watercolor * 0.3 * SunFade, watercolor, SunTransmittance);

    vec3 fogging = mix(refraction, watercolor * fogdarkness, saturate(fog / WaterExtinction)); // adding water color fog

    vec3 color;

    if (aboveWater)
    {
        color = mix(fogging, reflection, fresnel * 0.6);
    }
    // scatter and extinction between surface and camera
    else
    {
        color = mix(min(refraction * 1.2, 1.0), reflection, fresnel);
        color = mix(color, watercolor * darkness * ScatterFade, saturate(fog / WaterExtinction));
    }

    color += LightColor0 * 10.0 * specular;

    if (aboveWater)
    {
        color = ApplyFog(color, FogParams.x, FogColour.rgb, surfaceDepth, vVec, LightDir0.xyz, CameraPosition);
    }

    FragColor = (color);
}

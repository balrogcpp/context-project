// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"
#ifndef GL_ES
#define SPHERICAL_HARMONICS_BANDS 8
#else
#define SPHERICAL_HARMONICS_BANDS 3
#endif

#ifdef HAS_BASECOLORMAP
uniform sampler2D AlbedoTex;
#endif
#ifdef HAS_NORMALMAP
uniform sampler2D NormalTex;
#endif
#ifdef HAS_ORM
uniform sampler2D OrmTex;
#endif
#ifdef HAS_EMISSIVEMAP
uniform sampler2D EmissiveTex;
#endif
#ifdef HAS_IBL
uniform samplerCube SpecularEnvTex;
#endif
#ifdef TERRA_NORMALMAP
uniform sampler2D TerraNormalTex;
#endif
#ifdef TERRA_LIGHTMAP
uniform sampler2D TerraLightTex;
#endif
#if MAX_SHADOW_TEXTURES > 0
uniform sampler2D ShadowTex0;
uniform sampler2D ShadowTex1;
uniform sampler2D ShadowTex2;
uniform sampler2D ShadowTex3;
uniform sampler2D ShadowTex4;
uniform sampler2D ShadowTex5;
uniform sampler2D ShadowTex6;
uniform sampler2D ShadowTex7;
#endif

uniform vec3 iblSH[9];
uniform highp mat4 ViewMatrix;
uniform highp vec3 CameraPosition;
uniform float LightCount;
#if MAX_LIGHTS > 0
uniform highp vec4 LightPositionArray[MAX_LIGHTS];
uniform vec4 LightDirectionArray[MAX_LIGHTS];
uniform vec4 LightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 LightAttenuationArray[MAX_LIGHTS];
uniform vec4 LightSpotParamsArray[MAX_LIGHTS];
#endif // MAX_LIGHTS > 0
uniform vec4 AmbientLightColour;
uniform vec4 SurfaceAmbientColour;
uniform vec4 SurfaceDiffuseColour;
uniform vec4 SurfaceSpecularColour;
uniform float SurfaceShininessColour;
uniform vec4 SurfaceEmissiveColour;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform float TexScale;
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP)
uniform float OffsetScale;
#endif
#if MAX_SHADOW_TEXTURES > 0
uniform vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
uniform vec4 PssmSplitPoints;
uniform vec4 ShadowColour;

#include "pssm.glsl"
#endif

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 Diffuse(vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}

float pow5(float x)
{
    float x2 = x * x;
    return x2 * x2 * x;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 SpecularReflection(vec3 reflectance0, vec3 reflectance90, float VdotH)
{
    return reflectance0 + (reflectance90 - reflectance0) * pow5(1.0 - VdotH);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float GeometricOcclusion(float NdotL, float NdotV, float r)
{
    float r2 = (r * r);
    float r3 = (1.0 - r2);
    float attenuationL = (2.0 * NdotL) / (NdotL + sqrt(r2 + r3 * (NdotL * NdotL)));
    float attenuationV = (2.0 * NdotV) / (NdotV + sqrt(r2 + r3 * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
highp float MicrofacetDistribution(highp float alphaRoughness, highp float NdotH)
{
    highp float roughnessSq = alphaRoughness * alphaRoughness;
    highp float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
    return roughnessSq / (M_PI * (f * f));
}

// Chan 2018, "Material Advances in Call of Duty: WWII"
float ComputeMicroShadowing(float NdotL, float visibility) {
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NdotL * aperture);
    return microShadow * microShadow;
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(vec3 specularColor, float roughness, float NdotV)
{
    vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return specularColor * AB.x + AB.y;
}

float EnvBRDFApproxNonmetal(float roughness, float NdotV)
{
    // Same as EnvBRDFApprox( 0.04, Roughness, NoV )
    vec2 c0 = vec2(-1.0, -0.0275);
    vec2 c1 = vec2(1.0, 0.0425);
    vec2 r = roughness * c0 + c1;
    return min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
}

// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
vec3 Irradiance_SphericalHarmonics(vec3 iblSH[9], vec3 n) {
    return max(
    iblSH[0]
#if SPHERICAL_HARMONICS_BANDS >= 2
    + iblSH[1] * (n.y)
    + iblSH[2] * (n.z)
    + iblSH[3] * (n.x)
#endif
#if SPHERICAL_HARMONICS_BANDS >= 3
    + iblSH[4] * (n.y * n.x)
    + iblSH[5] * (n.y * n.z)
    + iblSH[6] * (3.0 * n.z * n.z - 1.0)
    + iblSH[7] * (n.z * n.x)
    + iblSH[8] * (n.x * n.x - n.y * n.y)
#endif
    , 0.0);
}

vec3 Irradiance_RoughnessOne(samplerCube SpecularEnvTex, vec3 n) {
    // note: lod used is always integer, hopefully the hardware skips tri-linear filtering
    return textureCubeLod(SpecularEnvTex, n, 9.0).rgb;
}

#ifdef HAS_IBL
vec3 DiffuseIrradiance(vec3 n)
{
    if (iblSH[0].r >= HALF_MAX_MINUS1) {
        return LINEARtoSRGB(textureCubeLod(SpecularEnvTex, n, 9.0).rgb);
    } else {
        return Irradiance_SphericalHarmonics(iblSH, n);
    }
}

vec3 GetIblSpeculaColor(vec3 reflection, float perceptualRoughness)
{
    return LINEARtoSRGB(LINEARtoSRGB(textureCubeLod(SpecularEnvTex, reflection, perceptualRoughness * 9.0).rgb));
}
#endif // HAS_IBL

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct Light
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
};

struct PBRInfo
{
    vec2 uv;
    float NdotV;                  // cos angle between normal and view direction
    vec3 reflection;
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    float occlusion;
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};


// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 EvaluateIBL(PBRInfo pbr)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = EnvBRDFApprox(pbr.specularColor, pbr.perceptualRoughness, pbr.NdotV);

#ifdef HAS_IBL
    vec3 diffuseLight = DiffuseIrradiance(pbr.reflection);
    vec3 specularLight = GetIblSpeculaColor(pbr.reflection, pbr.perceptualRoughness);
#else
    vec3 diffuseLight = Irradiance_SphericalHarmonics(iblSH, pbr.reflection);
    vec3 specularLight = diffuseLight;
#endif

    vec3 diffuse = diffuseLight * pbr.diffuseColor;
    vec3 specular = specularLight * (pbr.specularColor * brdf.x + brdf.y);
    return diffuse + specular;
}

vec3 SurfaceShading(Light light, PBRInfo pbr)
{
    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = SpecularReflection(pbr.reflectance0, pbr.reflectance90, light.VdotH);
    vec3 diffuseContrib = (1.0 - F) * Diffuse(pbr.diffuseColor);

    // Calculation of analytical lighting contribution
    float G = GeometricOcclusion(light.NdotL, pbr.NdotV, pbr.alphaRoughness);
    float D = MicrofacetDistribution(pbr.alphaRoughness, light.NdotH);
    vec3 specContrib = (F * (G * D)) / (4.0 * (light.NdotL * pbr.NdotV));

    return diffuseContrib + specContrib;
}

float GetAttenuation(int index, vec3 lightView)
{
    vec4 attParams = LightAttenuationArray[index];
    vec4 spotParams = LightSpotParamsArray[index];
    highp float range = attParams.x;
    highp float fLightD = length(lightView);
    if (fLightD > range) return 0.0;

    highp float fLightD2 = fLightD * fLightD;
    highp vec3 vLightView = normalize(lightView);
    float attenuationConst = attParams.y;
    float attenuationLinear = attParams.z;
    float attenuationQuad = attParams.w;
    float attenuation = 1.0 / (attenuationConst + (attenuationLinear * fLightD) + (attenuationQuad * fLightD2));

    // spotlight
    if (spotParams.w != 0.0) {
        float outerRadius = spotParams.z;
        float fallof = spotParams.x;
        float innerRadius = spotParams.y;

        highp vec3 l = -normalize(LightDirectionArray[index].xyz); // Vector from surface point to light
        float rho = dot(l, vLightView);
        float fSpotE = saturate((rho - innerRadius) / (fallof - innerRadius));
        attenuation *= pow(fSpotE, outerRadius);
    }

    return attenuation;
}

vec3 EvaluateDirectionalLight(PBRInfo pbr, highp vec3 v, highp vec3 n, highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES])
{
    highp vec3 l = -normalize(LightDirectionArray[0].xyz); // Vector from surface point to light
    highp float NdotL = saturate(dot(n, l));
    if (NdotL <= 0.001) return vec3(0.0, 0.0, 0.0);
    highp vec3 h = normalize(l + v); // Half vector between both l and v

    Light light;
    light.LdotH = dot(l, h);
    light.NdotH = dot(n, h);
    light.NdotL = NdotL;
    light.VdotH = dot(v, h);
    vec3 color = SurfaceShading(light, pbr) * NdotL * ComputeMicroShadowing(NdotL, pbr.occlusion);
#ifdef TERRA_LIGHTMAP
    color *= FetchTerraShadow(TerraLightTex, pbr.uv);
#endif

#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
#if PSSM_SPLIT_COUNT == 1
        color *= saturate(CalcShadow(lightSpacePosArray[0], 0) + ShadowColour.r);
#elif PSSM_SPLIT_COUNT > 1
        color *= saturate(CalcPSSMShadow(lightSpacePosArray) + ShadowColour.r);
#endif
    }
#endif

    return LightDiffuseScaledColourArray[0].xyz * color;
}

vec3 EvaluateLocalLights(PBRInfo pbr, highp vec3 v, highp vec3 n, vec3 worldPosition, highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES])
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        highp vec4 lightPosition = LightPositionArray[i];
        if (lightPosition.w == 0.0) continue;
        highp vec3 l = -normalize(LightDirectionArray[i].xyz); // Vector from surface point to light
        highp float NdotL = saturate(dot(n, l));
        if (NdotL <= 0.001) continue;

        // attenuation is property of spot and point light
        float attenuation = GetAttenuation(i, lightPosition.xyz - worldPosition);
        if (attenuation == 0.0) continue;

        Light light;
        highp vec3 h = normalize(l + v); // Half vector between both l and v
        light.LdotH = dot(l, h);
        light.NdotH = dot(n, h);
        light.NdotL = NdotL;
        light.VdotH = dot(v, h);
        vec3 c = SurfaceShading(light, pbr) * NdotL * attenuation * ComputeMicroShadowing(NdotL, pbr.occlusion);

#if MAX_SHADOW_TEXTURES > 0
        if (LightCastsShadowsArray[0] != 0.0) {
            c *= saturate(CalcShadow(lightSpacePosArray[i], i) + ShadowColour.r);
        }
#endif

        color += LightDiffuseScaledColourArray[0].xyz * c;
    }

    return color;
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
highp vec3 GetNormal(vec2 uv, highp mat3 tbn, vec2 uv1)
{
#ifdef TERRA_NORMALMAP
    highp vec3 t = vec3(1.0, 0.0, 0.0);
    highp vec3 ng = texture2D(TerraNormalTex, uv1).xyz * 2.0 - 1.0;
    highp vec3 b = normalize(cross(ng, t));
    t = normalize(cross(ng ,b));
    tbn = mtxFromCols3x3(t, b, ng);

#ifdef HAS_NORMALMAP
    return normalize(mul(tbn, (2.0 * SurfaceSpecularColour.a * texture2D(NormalTex, uv).xyz - 1.0)));
#else
    return tbn[2].xyz;
#endif // HAS_NORMALMAP
#else

#ifdef HAS_NORMALMAP
    return normalize(mul(tbn, (2.0 * SurfaceSpecularColour.a * texture2D(NormalTex, uv).xyz - 1.0)));
#else
    return tbn[2].xyz;
#endif // HAS_NORMALMAP
#endif // TERRA_NORMALMAP
}

// Sampler helper functions
vec4 GetAlbedo(vec2 uv, vec4 color)
{
    vec4 albedo = SurfaceDiffuseColour * color;
#ifdef HAS_BASECOLORMAP
    albedo *= texture2D(AlbedoTex, uv);
#endif
#ifdef HAS_ALPHA
    if (albedo.a < 0.5) discard;
#endif

    return vec4(SRGBtoLINEAR(albedo.rgb), albedo.a);
}

vec3 GetEmission(vec2 uv)
{
    vec3 emission = SurfaceEmissiveColour.rgb;
#ifdef HAS_EMISSIVEMAP
     if (textureSize(EmissiveTex, 0).x > 1) emission += SRGBtoLINEAR(SurfaceSpecularColour.b * texture2D(EmissiveTex, uv).rgb);
#endif
    return emission;
}

vec3 GetORM(vec2 uv, float spec)
{
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to pbr roughness
#ifndef TERRA_NORMALMAP
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r * saturate(1.0 - spec/128.0), 0.0);
    // vec3 orm = vec3(1.0, SurfaceSpecularColour.r * saturate(1.0 - 0.25 * pow(spec, 0.2)), 0.0);
#endif
#ifdef HAS_ORM
    if (textureSize(OrmTex, 0).x > 1) orm *= texture2D(OrmTex, uv).rgb;
    else orm.b = 0.0;
#endif

    return clamp(orm, vec3(0.0, F0, 0.0), vec3(1.0, 1.0, 1.0));
}

vec2 GetParallaxCoord(highp vec2 uv0, highp vec3 v)
{
    vec2 uv = uv0 * (1.0 + TexScale);
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP)
    uv = uv - (vec2(v.x, -v.y) * OffsetScale * texture2D(NormalTex, uv).a);
#endif
    return uv;
}

in highp vec3 vWorldPosition;
in highp mat3 vTBN;
in vec2 vUV0;
in vec4 vColor;
in vec4 vScreenPosition;
in vec4 vPrevScreenPosition;
#if MAX_SHADOW_TEXTURES > 0
in highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
void main()
{
    highp vec3 v = normalize(CameraPosition - vWorldPosition);
    vec2 uv = GetParallaxCoord(vUV0, v);
    vec4 colour = GetAlbedo(uv, vColor);
    vec3 orm = GetORM(uv, colour.a);
    vec3 emission = GetEmission(uv);
    highp vec3 n = GetNormal(uv, vTBN, vUV0);
    vec3 albedo = colour.rgb;
    float alpha = colour.a;
    float roughness = orm.g;
    float metallic = orm.b;
    float occlusion = orm.r;

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    vec3 diffuseColor = albedo * (1.0 - F0) * (1.0 - metallic);
    vec3 specularColor = mix(vec3(F0, F0, F0), albedo, metallic);
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    PBRInfo pbr;
    pbr.uv = vUV0;
    pbr.NdotV = abs(dot(n, v)) + 0.001;
    pbr.diffuseColor = diffuseColor;
    pbr.specularColor = specularColor;
    pbr.metalness = metallic;
    pbr.occlusion = occlusion;
    pbr.perceptualRoughness = roughness;
    pbr.alphaRoughness = roughness * roughness;
    pbr.reflectance0 = specularColor.rgb;
    pbr.reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    pbr.reflection = -normalize(reflect(v, n));

    vec3 color = vec3(0.0, 0.0, 0.0);
    color += SurfaceAmbientColour.rgb * (AmbientLightColour.rgb * EvaluateIBL(pbr));
    color += EvaluateDirectionalLight(pbr, v, n, vLightSpacePosArray);
    color += EvaluateLocalLights(pbr, v, n, vWorldPosition, vLightSpacePosArray);

    // Apply optional PBR terms for additional (optional) shading
    color += emission;
    color = ApplyFog(color, FogParams, FogColour.rgb, vScreenPosition.z);

    EvaluateBuffer(color, alpha);
#ifdef HAS_MRT
    FragData[MRT_NORMALS].xyz = normalize(mul(ViewMatrix, vec4(n, 0.0)).xyz);
    FragData[MRT_GLOSS].rgb = vec3(metallic, roughness, alpha);
    if (Any(vPrevScreenPosition.xz)) FragData[MRT_VELOCITY].xy = (vScreenPosition.xz / vScreenPosition.w) - (vPrevScreenPosition.xz / vPrevScreenPosition.w);
#endif
}

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
#endif // HAS_BASECOLORMAP
#ifdef HAS_NORMALMAP
uniform sampler2D NormalTex;
#endif // HAS_NORMALMAP
#ifdef HAS_ORM
uniform sampler2D OrmTex;
#endif // HAS_ORM
#ifdef HAS_EMISSIVEMAP
uniform sampler2D EmissiveTex;
#endif // HAS_EMISSIVEMAP
#ifdef HAS_IBL
uniform samplerCube SpecularEnvTex;
#endif // HAS_IBL
#ifdef TERRA_NORMALMAP
uniform sampler2D TerraNormalTex;
#endif // TERRA_NORMALMAP
#ifdef TERRA_LIGHTMAP
uniform sampler2D TerraLightTex;
#endif
#if MAX_SHADOW_TEXTURES > 0
#if MAX_SHADOW_TEXTURES > 0
uniform sampler2D ShadowTex0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform sampler2D ShadowTex1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform sampler2D ShadowTex2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform sampler2D ShadowTex3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform sampler2D ShadowTex4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform sampler2D ShadowTex5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform sampler2D ShadowTex6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform sampler2D ShadowTex7;
#endif
#endif // MAX_SHADOW_TEXTURES > 0

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
#endif // MAX_SHADOW_TEXTURES > 0

#if MAX_SHADOW_TEXTURES > 0
#include "pssm.glsl"
#endif


// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 Diffuse(const vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 SpecularReflection(const vec3 reflectance0, const vec3 reflectance90, const float VdotH)
{
    return reflectance0 + (reflectance90 - reflectance0) * pow5(1.0 - VdotH);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float GeometricOcclusion(const float NdotL, const float NdotV, const float r)
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
highp float MicrofacetDistribution(const highp float alphaRoughness, const highp float NdotH)
{
    highp float roughnessSq = alphaRoughness * alphaRoughness;
    highp float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
    return roughnessSq / (M_PI * (f * f));
}

// Chan 2018, "Material Advances in Call of Duty: WWII"
float ComputeMicroShadowing(const float NdotL, const float visibility) {
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NdotL * aperture);
    return microShadow * microShadow;
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(const vec3 specularColor, const float roughness, const float NdotV)
{
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return specularColor * AB.x + AB.y;
}

float EnvBRDFApproxNonmetal(const float roughness, const float NdotV)
{
    // Same as EnvBRDFApprox( 0.04, Roughness, NoV )
    const vec2 c0 = vec2(-1.0, -0.0275);
    const vec2 c1 = vec2(1.0, 0.0425);
    vec2 r = roughness * c0 + c1;
    return min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
}

// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
vec3 Irradiance_SphericalHarmonics(const vec3 iblSH[9], const vec3 n) {
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

vec3 Irradiance_RoughnessOne(samplerCube SpecularEnvTex, const vec3 n) {
    // note: lod used is always integer, hopefully the hardware skips tri-linear filtering
    return textureCubeLod(SpecularEnvTex, n, 9.0).rgb;
}

#ifdef HAS_IBL
vec3 DiffuseIrradiance(const vec3 n)
{
    if (iblSH[0].r >= HALF_MAX_MINUS1) {
        return LINEARtoSRGB(textureCubeLod(SpecularEnvTex, n, 9.0).rgb);
    } else {
        return Irradiance_SphericalHarmonics(iblSH, n);
    }
}

vec3 GetIblSpeculaColor(const vec3 reflection, const float perceptualRoughness)
{
//#ifdef HAS_MRT
    return LINEARtoSRGB(LINEARtoSRGB(textureCubeLod(SpecularEnvTex, reflection, perceptualRoughness * 9.0).rgb));
//#else
//    return LINEARtoSRGB(textureCubeLod(SpecularEnvTex, reflection, perceptualRoughness * 9.0).rgb);
//#endif
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
    float NdotV;                  // cos angle between normal and view direction
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    float occlusion;
    float attenuation;
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};


// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 GetIBL(const PBRInfo pbr, const vec3 reflection)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = EnvBRDFApprox(pbr.specularColor, pbr.perceptualRoughness, pbr.NdotV);

#ifdef HAS_IBL
    vec3 diffuseLight = DiffuseIrradiance(reflection);
    vec3 specularLight = GetIblSpeculaColor(reflection, pbr.perceptualRoughness);
#else
    vec3 diffuseLight = Irradiance_SphericalHarmonics(iblSH, reflection);
    vec3 specularLight = diffuseLight;
#endif

    vec3 diffuse = diffuseLight * pbr.diffuseColor;
    vec3 specular = specularLight * (pbr.specularColor * brdf.x + brdf.y);
    return diffuse + specular;
}

vec3 GetDirectionalLight(const highp vec3 v, const highp vec3 n, const highp vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES], const PBRInfo pbr)
{
    highp vec3 l = -normalize(LightDirectionArray[0].xyz); // Vector from surface point to light
    highp float NdotL = saturate(dot(n, l));
    if (NdotL < 0.001) return vec3(0.0, 0.0, 0.0);

    highp vec3 h = normalize(l + v); // Half vector between both l and v

    // attenuation is property of spot and point light
    highp vec4 vLightPositionArray = LightPositionArray[0];
    float light = NdotL * ComputeMicroShadowing(NdotL, pbr.occlusion);
    highp float NdotH = dot(n, h);
    float LdotH = dot(l, h);
    float VdotH = dot(v, h);

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = SpecularReflection(pbr.reflectance0, pbr.reflectance90, VdotH);
    vec3 diffuseContrib = (1.0 - F) * Diffuse(pbr.diffuseColor);

    // Calculation of analytical lighting contribution
    float G = GeometricOcclusion(NdotL, pbr.NdotV, pbr.alphaRoughness);
    float D = MicrofacetDistribution(pbr.alphaRoughness, NdotH);
    vec3 specContrib = (F * (G * D)) / (4.0 * (NdotL * pbr.NdotV));

    light *= pbr.attenuation;

#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
        light *= saturate(CalcPSSMShadow(lightSpacePosArray) + ShadowColour.r);
    }
#endif

    return LightDiffuseScaledColourArray[0].xyz * (light * (diffuseContrib + specContrib));
}

vec3 GetLocalLights(const highp vec3 v, const highp vec3 n, const vec3 vWorldPosition, const highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES], const PBRInfo pbr)
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 1; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        highp vec3 l = -normalize(LightDirectionArray[0].xyz); // Vector from surface point to light
        highp float NdotL = saturate(dot(n, l));
        if (NdotL < 0.001) continue;

        highp vec3 h = normalize(l + v); // Half vector between both l and v

        // attenuation is property of spot and point light
        highp vec4 vLightPositionArray = LightPositionArray[i];
        float attenuation = 1.0;
        if (vLightPositionArray.w != 0.0) {
            vec4 vAttParams = LightAttenuationArray[i];
            highp float range = vAttParams.x;
            highp vec3 vLightViewH = vLightPositionArray.xyz - vWorldPosition;
            highp float fLightD = length(vLightViewH);
            highp float fLightD2 = fLightD * fLightD;
            highp vec3 vLightView = normalize(vLightViewH);
            float attenuation_const = vAttParams.y;
            float attenuation_linear = vAttParams.z;
            float attenuation_quad = vAttParams.w;
            attenuation = biggerhp(range, fLightD) / (attenuation_const + (attenuation_linear * fLightD) + (attenuation_quad * fLightD2));

            // spotlight
            vec4 vSpotParams = LightSpotParamsArray[i];
            if (vSpotParams.w != 0.0) {
                float outerRadius = vSpotParams.z;
                float fallof = vSpotParams.x;
                float innerRadius = vSpotParams.y;

                float rho = dot(l, vLightView);
                float fSpotE = saturate((rho - innerRadius) / (fallof - innerRadius));
                attenuation *= pow(fSpotE, outerRadius);
            }
        }

        float light = NdotL;
        highp float NdotH = dot(n, h);
        float LdotH = dot(l, h);
        float VdotH = dot(v, h);

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = SpecularReflection(pbr.reflectance0, pbr.reflectance90, VdotH);
        vec3 diffuseContrib = (1.0 - F) * Diffuse(pbr.diffuseColor);

        // Calculation of analytical lighting contribution
        float G = GeometricOcclusion(NdotL, pbr.NdotV, pbr.alphaRoughness);
        float D = MicrofacetDistribution(pbr.alphaRoughness, NdotH);
        vec3 specContrib = (F * (G * D)) / (4.0 * (NdotL * pbr.NdotV));

        light *= attenuation;
        #if MAX_SHADOW_TEXTURES > 0
        if (LightCastsShadowsArray[0] != 0.0) {
            light *= saturate(CalcShadow(vLightSpacePosArray[i], i) + ShadowColour.r);
        }
        #endif

        color += LightDiffuseScaledColourArray[0].xyz * (light * (diffuseContrib + specContrib));
    }

    return color;
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
highp vec3 GetNormal(const vec2 uv, highp mat3 tbn, const vec2 uv1)
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
vec4 GetAlbedo(const vec2 uv, const vec4 color)
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

vec3 GetEmission(const vec2 uv)
{
    vec3 emission = SurfaceEmissiveColour.rgb;
#ifdef HAS_EMISSIVEMAP
     if (textureSize(EmissiveTex, 0).x > 1) emission += SRGBtoLINEAR(SurfaceSpecularColour.b * texture2D(EmissiveTex, uv).rgb);
#endif
    return emission;
}

vec3 GetORM(const vec2 uv, const float spec)
{
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to pbr roughness
#ifndef TERRA_NORMALMAP
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    //vec3 orm = vec3(1.0, SurfaceSpecularColour.r * saturate(1.0 - spec/128.0), 0.0);
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r * saturate(1.0 - 0.25 * pow(spec, 0.2)), 0.0);
#endif
#ifdef HAS_ORM
    if (textureSize(OrmTex, 0).x > 1) orm *= texture2D(OrmTex, uv).rgb;
    else orm.b = 0.0;
#endif

    return clamp(orm, vec3(0.0, F0, 0.0), vec3(1.0, 1.0, 1.0));
}

vec2 GetParallaxCoord(const highp vec2 uv0, const highp vec3 v)
{
    highp vec2 uv = uv0 * (1.0 + TexScale);
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP)
    uv = uv - (vec2(v.x, -v.y) * OffsetScale * texture2D(NormalTex, uv).a);
#endif
    return uv;
}

in highp vec3 vWorldPosition;
in highp mat3 vTBN;
in highp vec2 vUV0;
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
    vec3 diffuseColor = albedo * ((1.0 - F0) * (1.0 - metallic));
    vec3 specularColor = mix(vec3(F0, F0, F0), albedo, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    vec3 reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    vec3 reflectance0 = specularColor.rgb;
    float NdotV = clamp(dot(n, v), 0.001, 1.0);

    PBRInfo pbr;
    pbr.NdotV = NdotV;
    pbr.diffuseColor = diffuseColor;
    pbr.specularColor = specularColor;
    pbr.metalness = metallic;
    pbr.occlusion = occlusion;
    pbr.perceptualRoughness = roughness;
    pbr.alphaRoughness = roughness * roughness;
    pbr.reflectance0 = reflectance0;
    pbr.reflectance90 = reflectance90;
    pbr.attenuation = 1.0;
#ifdef TERRA_LIGHTMAP
    pbr.attenuation = FetchTerraShadow(TerraLightTex, vUV0);
#endif

    vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 reflection = -normalize(reflect(v, n));
    color += SurfaceAmbientColour.rgb * (AmbientLightColour.rgb * GetIBL(pbr, reflection));
    color += GetDirectionalLight(v, n, vLightSpacePosArray, pbr);
    color += GetLocalLights(v, n, vWorldPosition, vLightSpacePosArray, pbr);

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

// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"

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
#ifdef HAS_AO
uniform sampler2D OcclusionTex;
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

uniform vec3 iblSH[9];
uniform highp mat4 ViewMatrix;
uniform highp vec3 CameraPosition;
uniform vec4 ViewportSize;
uniform float LightCount;
#if MAX_LIGHTS > 0
uniform highp vec4 LightPositionArray[MAX_LIGHTS];
uniform vec4 LightDirectionArray[MAX_LIGHTS];
uniform vec4 LightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 LightAttenuationArray[MAX_LIGHTS];
uniform vec4 LightSpotParamsArray[MAX_LIGHTS];
#endif // MAX_LIGHTS > 0
uniform float SurfaceAlphaRejection;
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
vec3 SpecularReflection(const vec3 reflectance0, const vec3 reflectance90, float VdotH)
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
float MicrofacetDistribution(float roughness, float NdotH, float oneMinusNoHSquared)
{
    float a = NdotH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / M_PI);
    return SafeHDR(d);
}

// Chan 2018, "Material Advances in Call of Duty: WWII"
float ComputeMicroShadowing(float NdotL, float visibility)
{
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NdotL * aperture);
    return microShadow * microShadow;
}

float computeSpecularAO(float NdotV, float visibility, float roughness)
{
    return saturate(pow(NdotV + visibility, exp2(-16.0 * roughness - 1.0)) - 1.0 + visibility);
}

/**
 * Returns a color ambient occlusion based on a pre-computed visibility term.
 * The albedo term is meant to be the diffuse color or f0 for the diffuse and
 * specular terms respectively.
 */
vec3 gtaoMultiBounce(float visibility, const vec3 albedo)
{
    // Jimenez et al. 2016, "Practical Realtime Strategies for Accurate Indirect Occlusion"
    vec3 a =  2.0404 * albedo - 0.3324;
    vec3 b = -4.7951 * albedo + 0.6417;
    vec3 c =  2.7552 * albedo + 0.6903;

    return max(vec3(visibility), ((visibility * a + b) * visibility + c) * visibility);
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(const vec3 specularColor, float roughness, float NdotV)
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

#ifdef HAS_IBL
// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
vec3 Irradiance_SphericalHarmonics(const vec3 n) {
    return max(
        iblSH[0]
        + iblSH[1] * (n.y)
        + iblSH[2] * (n.z)
        + iblSH[3] * (n.x)
        + iblSH[4] * (n.y * n.x)
        + iblSH[5] * (n.y * n.z)
        + iblSH[6] * (3.0 * n.z * n.z - 1.0)
        + iblSH[7] * (n.z * n.x)
        + iblSH[8] * (n.x * n.x - n.y * n.y)
        , 0.0);
}

vec3 DiffuseIrradiance(const vec3 n)
{
    return textureCube(SpecularEnvTex, n).rgb;
}

vec3 GetIblSpecularColor(const vec3 n)
{
    return LINEARtoSRGB(LINEARtoSRGB(textureCube(SpecularEnvTex, n).rgb));
}
#endif // HAS_IBL

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct Light
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotH;                  // cos angle between normal and half vector
    float NxH;
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

vec3 v, n;

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 EvaluateIBL(const PBRInfo material)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = EnvBRDFApprox(material.specularColor, material.perceptualRoughness, material.NdotV);
    float diffuseAO = material.occlusion;
    float specularAO = computeSpecularAO(material.NdotV, diffuseAO, material.perceptualRoughness);

#ifdef HAS_IBL
    vec3 diffuseLight = Irradiance_SphericalHarmonics(material.reflection);
    vec3 specularLight = GetIblSpecularColor(material.reflection);
#else
    const vec3 diffuseLight = vec3(1.0, 1.0, 1.0);
    vec3 specularLight = diffuseLight;
#endif

    vec3 diffuse = diffuseLight * material.diffuseColor;
    vec3 specular = specularLight * (material.specularColor * brdf.x + brdf.y);
    if (diffuseAO == 1.0) return SurfaceAmbientColour.rgb * AmbientLightColour.rgb * (diffuse + specular);
    return SurfaceAmbientColour.rgb * AmbientLightColour.rgb * (diffuse * gtaoMultiBounce(diffuseAO, material.diffuseColor) + specular * gtaoMultiBounce(specularAO, material.reflectance0));
}

vec3 SurfaceShading(const Light light, const PBRInfo material)
{
    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = SpecularReflection(material.reflectance0, material.reflectance90, light.VdotH);
    vec3 diffuseContrib = (1.0 - F) * Diffuse(material.diffuseColor);

    // Calculation of analytical lighting contribution
    float G = GeometricOcclusion(light.NdotL, material.NdotV, material.alphaRoughness);
    float D = MicrofacetDistribution(material.alphaRoughness, light.NdotH, light.NxH);
    vec3 specContrib = (F * G * D) / (4.0 * light.NdotL * material.NdotV);

    return diffuseContrib + specContrib;
}

float GetAttenuation(int index, const vec3 lightView)
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

vec3 EvaluateDirectionalLight(const PBRInfo material, const vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES])
{
    vec3 l = -normalize(LightDirectionArray[0].xyz); // Vector from surface point to light
    float NdotL = saturate(dot(n, l));
    if (NdotL <= 0.001) return vec3(0.0, 0.0, 0.0);
    float attenuation = 1.0;

#ifdef TERRA_LIGHTMAP
    attenuation = saturate(FetchTerraShadow(TerraLightTex, material.uv) + ShadowColour.r);
    if (attenuation == 0.0) return vec3(0.0, 0.0, 0.0);
#endif
#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
#if PSSM_SPLIT_COUNT <= 1
        attenuation = saturate(CalcShadow(vec4(lightSpacePosArray[0].xy * 0.5, lightSpacePosArray[0].zw), 0) + ShadowColour.r);
#elif PSSM_SPLIT_COUNT > 1
        attenuation = saturate(CalcPSSMShadow(lightSpacePosArray) + ShadowColour.r);
#endif
        if (attenuation == 0.0) return vec3(0.0, 0.0, 0.0);
    }
#endif

    vec3 h = normalize(l + v); // Half vector between both l and v

    Light light;
    light.LdotH = dot(l, h);
    light.NdotH = dot(n, h);
    vec3 NxH = cross(n, h);
    light.NxH = dot(NxH, NxH);
    light.NdotL = NdotL;
    light.VdotH = dot(v, h);
    vec3 color = SurfaceShading(light, material) * ComputeMicroShadowing(NdotL, material.occlusion) * NdotL;
    return LightDiffuseScaledColourArray[0].xyz * color * attenuation;
}

vec3 EvaluateLocalLights(PBRInfo material, const vec3 worldPosition, const vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES])
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        highp vec4 lightPosition = LightPositionArray[i];
        if (lightPosition.w == 0.0) continue;
        vec3 l = -normalize(LightDirectionArray[i].xyz); // Vector from surface point to light
        float NdotL = saturate(dot(n, l));
        if (NdotL <= 0.001) continue;

        // attenuation is property of spot and point light
        float attenuation = GetAttenuation(i, lightPosition.xyz - worldPosition);
        if (attenuation == 0.0) continue;

        Light light;
        vec3 h = normalize(l + v); // Half vector between both l and v
        light.LdotH = saturate(dot(l, h));
        light.NdotH = saturate(dot(n, h));
        light.NdotL = NdotL;
        light.VdotH = saturate(dot(v, h));
        vec3 c = SurfaceShading(light, material) * NdotL * ComputeMicroShadowing(NdotL, material.occlusion);

#if MAX_SHADOW_TEXTURES > 1
        if (LightCastsShadowsArray[0] != 0.0) {
            attenuation *= saturate(CalcShadow(lightSpacePosArray[i], i) + ShadowColour.r);
            if (attenuation == 0.0) continue;
        }
#endif

        color += LightDiffuseScaledColourArray[0].xyz * c * attenuation;
    }

    return color;
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 GetNormal(highp mat3 tbn, const vec2 uv, const vec2 uv1)
{
#ifdef TERRA_NORMALMAP
    highp vec3 t = vec3(1.0, 0.0, 0.0);
    highp vec3 ng = texture2D(TerraNormalTex, uv1).xyz * 2.0 - 1.0;
    highp vec3 b = normalize(cross(ng, t));
    t = normalize(cross(ng ,b));
    tbn = mtxFromCols3x3(t, b, ng);

#ifdef HAS_NORMALMAP
    vec3 normal = texture2D(NormalTex, uv).xyz;
    return normalize(mul(tbn, (2.0 * SurfaceSpecularColour.a * normal - 1.0)));
#else
    return tbn[2].xyz;
#endif // HAS_NORMALMAP
#else

#ifdef HAS_NORMALMAP
    vec3 normal = texture2D(NormalTex, uv).xyz;
    return normalize(mul(tbn, (2.0 * SurfaceSpecularColour.a * normal - 1.0)));
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

    if (albedo.a < SurfaceAlphaRejection) discard;
    return vec4(SRGBtoLINEAR(albedo.rgb), albedo.a);
}

vec3 GetEmission(const vec2 uv)
{
    vec3 emission = SurfaceEmissiveColour.rgb;
#ifdef HAS_EMISSIVEMAP
    if (textureSize(EmissiveTex, 0).x > 1) emission += texture2D(EmissiveTex, uv).rgb;
#endif
    return SRGBtoLINEAR(emission) * SurfaceSpecularColour.b;
}

vec3 GetORM(const vec2 uv, float spec)
{
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to material roughness
#ifndef TERRA_NORMALMAP
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    vec3 orm = vec3(1.0, SurfaceSpecularColour.r * saturate(1.0 - spec/128.0), 0.0);
#endif
#ifdef HAS_ORM
#ifdef HAS_ATLAS
    orm *= texture2D(AtlasTex, fract(uv) * 0.5 + vec2(0.0, 0.5)).rgb;
#else
    if (textureSize(OrmTex, 0).x > 1) orm *= texture2D(OrmTex, uv).rgb;
    else orm.b = 0.0;
#endif
#endif

    return clamp(orm, vec3(0.0, F0, 0.0), vec3(1.0, 1.0, 1.0));
}

vec2 GetParallaxCoord(const highp vec2 uv0, const highp vec3 v)
{
    vec2 uv = uv0 * (1.0 + TexScale);
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP)
    uv = uv - (vec2(v.x, -v.y) * (OffsetScale + 0.01) * texture2D(NormalTex, uv).a);
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
    v = normalize(CameraPosition - vWorldPosition);
    vec2 uv = GetParallaxCoord(vUV0, v);
    vec4 colour = GetAlbedo(uv, vColor);
    vec3 orm = GetORM(uv, colour.a);
    vec3 emission = GetEmission(uv);
    n = GetNormal(vTBN, uv, vUV0);
    vec3 albedo = colour.rgb;
    float alpha = colour.a;
    float roughness = orm.g;
    float metallic = orm.b;
    float occlusion = orm.r;
#ifdef HAS_AO
    float ao = texture2D(OcclusionTex, gl_FragCoord.xy * ViewportSize.zw).r;
    occlusion = min(occlusion, ao);
#endif

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    vec3 diffuseColor = albedo * (1.0 - F0) * (1.0 - metallic);
    vec3 specularColor = mix(vec3(F0, F0, F0), albedo, metallic);
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    PBRInfo material;
    material.uv = vUV0;
    material.NdotV = abs(dot(n, v)) + 0.001;
    material.diffuseColor = diffuseColor;
    material.specularColor = specularColor;
    material.metalness = metallic;
    material.occlusion = occlusion;
    material.perceptualRoughness = roughness;
    material.alphaRoughness = roughness * roughness;
    material.reflectance0 = specularColor.rgb;
    material.reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    material.reflection = -normalize(reflect(v, n));

    vec3 color = vec3(0.0, 0.0, 0.0);
    color += EvaluateIBL(material);
    color += EvaluateDirectionalLight(material, vLightSpacePosArray);
    color += EvaluateLocalLights(material, vWorldPosition, vLightSpacePosArray);
    color += emission;
    color = ApplyFog(color, FogParams, FogColour.rgb, vScreenPosition.z);

    EvaluateBuffer(color, alpha);
#ifdef HAS_MRT
    FragData[MRT_NORMALS].xyz = normalize(mul(ViewMatrix, vec4(n, 0.0)).xyz);
    FragData[MRT_GLOSS].rgb = vec3(metallic, roughness, alpha);
    if (Any(vPrevScreenPosition.xz)) FragData[MRT_VELOCITY].xy = (vScreenPosition.xz / vScreenPosition.w) - (vPrevScreenPosition.xz / vPrevScreenPosition.w);
#endif
}

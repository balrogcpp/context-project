// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "srgb.glsl"
#include "fog.glsl"

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
#ifdef HAS_SSR
uniform sampler2D SSR;
#endif
#if MAX_SHADOW_TEXTURES > 0
uniform sampler2D ShadowTex;
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

uniform vec4 TexSize0;
uniform vec4 TexSize1;
uniform vec4 TexSize2;
uniform vec4 TexSize3;
uniform vec4 TexSize4;
uniform vec4 TexSize5;
uniform vec4 TexSize6;
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform highp mat4 ViewMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp vec3 CameraPosition;
uniform vec4 ViewportSize;
uniform float LightCount;
uniform float StaticObj;
#if MAX_LIGHTS > 0
uniform highp vec4 LightPositionArray[MAX_LIGHTS];
uniform vec4 LightDirectionArray[MAX_LIGHTS];
uniform vec4 LightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 LightAttenuationArray[MAX_LIGHTS];
uniform vec4 LightSpotParamsArray[MAX_LIGHTS];
#endif // MAX_LIGHTS > 0
#if MAX_SHADOW_TEXTURES > 0
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
#endif
// uniform float SurfaceAlphaRejection;
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


#if MAX_SHADOW_TEXTURES > 0 || defined(TERRA_NORMALMAP)
#include "pssm.glsl"
#endif

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 Diffuse(const vec3 diffuseColor)
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

    return max(vec3(visibility, visibility, visibility), ((visibility * a + b) * visibility + c) * visibility);
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(const vec3 specularColor, float roughness, float NdotV)
{
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return specularColor * AB.x + AB.y;
}

float EnvBRDFApproxNonmetal(float roughness, float NdotV)
{
    // Same as EnvBRDFApprox( 0.04, Roughness, NoV )
    const vec2 c0 = vec2(-1.0, -0.0275);
    const vec2 c1 = vec2(1.0, 0.0425);
    vec2 r = roughness * c0 + c1;
    return min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
}

// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
#if SPHERICAL_HARMONICS_BANDS > 0
vec3 Irradiance_SphericalHarmonics(const vec3 n, const vec3 ibl[9]) {
    return max(
        ibl[0]
#if SPHERICAL_HARMONICS_BANDS >= 2
        + ibl[1] * (n.y)
        + ibl[2] * (n.z)
        + ibl[3] * (n.x)
#endif
#if SPHERICAL_HARMONICS_BANDS >= 3
        + ibl[4] * (n.y * n.x)
        + ibl[5] * (n.y * n.z)
        + ibl[6] * (3.0 * n.z * n.z - 1.0)
        + ibl[7] * (n.z * n.x)
        + ibl[8] * (n.x * n.x - n.y * n.y)
#endif
        , 0.0);
}
#endif

#ifdef HAS_IBL
vec3 decodeDataForIBL(const vec3 data)
{
#ifdef FORCE_TONEMAP
    return LINEARtoSRGB(LINEARtoSRGB(data));
#else
    return LINEARtoSRGB(data);
#endif
}

vec3 DiffuseIrradiance(const vec3 n)
{
    return decodeDataForIBL(textureCubeLod(SpecularEnvTex, vec3(n.x, n.y, n.z), 6.0).rgb);
}

vec3 GetIblSpecularColor(const vec3 n, float roughness)
{
    return decodeDataForIBL(textureCubeLod(SpecularEnvTex, vec3(n.x, n.y, -n.z), roughness * 6.0).rgb);
}
#endif // HAS_IBL

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct Light
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotH;                  // cos angle between normal and half vector
    float oneMinusNoHSquared;
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
    float ao;
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

vec3 v, n;
highp float fragDepth;

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 EvaluateIBL(const PBRInfo material)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = EnvBRDFApprox(material.specularColor, material.perceptualRoughness, material.NdotV);
    float diffuseAO = min(material.occlusion, material.ao);
    float specularAO = computeSpecularAO(material.NdotV, diffuseAO, material.perceptualRoughness);

#ifdef HAS_IBL
    vec3 diffuseLight = DiffuseIrradiance(material.reflection);
    vec3 specularLight = GetIblSpecularColor(material.reflection, material.perceptualRoughness);
#else
    const vec3 diffuseLight = vec3(1.0, 1.0, 1.0);
    vec3 specularLight = diffuseLight;
#endif

    vec3 diffuse = diffuseLight * material.diffuseColor;
    vec3 specular = specularLight * (material.specularColor * brdf.x + brdf.y);
    if (diffuseAO == 1.0) return SurfaceAmbientColour.rgb * AmbientLightColour.rgb * (diffuse + specular);
    return SurfaceAmbientColour.rgb * AmbientLightColour.rgb * (diffuse * gtaoMultiBounce(diffuseAO, material.diffuseColor) + specular * gtaoMultiBounce(specularAO, material.reflectance0));
}

#if MAX_LIGHTS > 0
vec3 SurfaceShading(const Light light, const PBRInfo material)
{
    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = SpecularReflection(material.reflectance0, material.reflectance90, light.VdotH);
    vec3 diffuseContrib = (1.0 - F) * Diffuse(material.diffuseColor);

    // Calculation of analytical lighting contribution
    float G = GeometricOcclusion(light.NdotL, material.NdotV, material.alphaRoughness);
    float D = MicrofacetDistribution(material.alphaRoughness, light.NdotH, light.oneMinusNoHSquared);
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

vec3 EvaluateDirectionalLight(const PBRInfo material
#if MAX_SHADOW_TEXTURES > 0
, const highp vec3 lightSpacePos0, const highp vec3 lightSpacePos1, const highp vec3 lightSpacePos2
#endif
)
{
    vec3 l = -normalize(LightDirectionArray[0].xyz); // Vector from surface point to light
    float NdotL = saturate(dot(n, l));
    if (NdotL <= 0.001) return vec3(0.0, 0.0, 0.0);
    float attenuation = 1.0;

#ifdef TERRA_LIGHTMAP
    attenuation = saturate(FetchTerraShadow(material.uv));
    if (attenuation == 0.0) return vec3(0.0, 0.0, 0.0);
#endif
#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
        attenuation *= saturate(CalcPSSMShadow(lightSpacePos0, lightSpacePos1, lightSpacePos2, fragDepth) + ShadowColour.r);
        if (attenuation == 0.0) return vec3(0.0, 0.0, 0.0);
    }
#endif

    vec3 h = normalize(l + v); // Half vector between both l and v

    Light light;
    light.LdotH = dot(l, h);
    light.NdotH = dot(n, h);
    // https://github.com/google/filament/blob/d273838e07c76be86a3521d32d8f51f3588e4292/shaders/src/brdf.fs#L54
#ifdef GL_ES
    vec3 NxH = cross(n, h);
    light.oneMinusNoHSquared = dot(NxH, NxH);
#else
    light.oneMinusNoHSquared = 1.0 - light.NdotH * light.NdotH;
#endif
    light.NdotL = NdotL;
    light.VdotH = dot(v, h);
    vec3 color = SurfaceShading(light, material) * ComputeMicroShadowing(NdotL, material.occlusion) * NdotL;
    return LightDiffuseScaledColourArray[0].xyz * color * attenuation;
}

vec3 EvaluateLocalLights(const PBRInfo material, const highp vec3 pixelWorldPosition
#if MAX_SHADOW_TEXTURES > 0
, const highp vec3 lightSpacePos[MAX_SHADOW_TEXTURES]
#endif
)
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
        float attenuation = GetAttenuation(i, lightPosition.xyz - pixelWorldPosition);
        if (attenuation == 0.0) continue;

        Light light;
        vec3 h = normalize(l + v); // Half vector between both l and v
        light.LdotH = saturate(dot(l, h));
        light.NdotH = saturate(dot(n, h));
        // https://github.com/google/filament/blob/d273838e07c76be86a3521d32d8f51f3588e4292/shaders/src/brdf.fs#L54
#ifdef GL_ES
        vec3 NxH = cross(n, h);
        light.oneMinusNoHSquared = dot(NxH, NxH);
#else
        light.oneMinusNoHSquared = 1.0 - light.NdotH * light.NdotH;
#endif
        light.NdotL = NdotL;
        light.VdotH = saturate(dot(v, h));
        vec3 c = SurfaceShading(light, material) * NdotL * ComputeMicroShadowing(NdotL, material.occlusion);

#if MAX_SHADOW_TEXTURES > 3
        if (LightCastsShadowsArray[0] != 0.0) {
            attenuation *= saturate(CalcShadow(lightSpacePos[i], i) + ShadowColour.r);
            if (attenuation == 0.0) continue;
        }
#endif

        color += LightDiffuseScaledColourArray[0].xyz * c * attenuation;
    }

    return color;
}
#endif

// Sampler helper functions
vec4 GetAlbedo(const vec2 uv, const vec3 color)
{
    vec4 albedo = vec4(SurfaceDiffuseColour.rgb * color, 1.0);
#ifdef HAS_BASECOLORMAP
    if (TexSize0.x > 1.0) albedo *= texture2D(AlbedoTex, uv);
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
    if (TexSize3.x > 1.0) emission += texture2D(EmissiveTex, uv).rgb;
#endif
    return SRGBtoLINEAR(emission) * SurfaceSpecularColour.b;
}

vec3 GetORM(const vec2 uv, float spec)
{
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to material roughness
#ifndef TERRA_NORMALMAP
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r * saturate(1.0 - spec/128.0), 0.0);
#endif
#ifdef HAS_ORM
    if (TexSize2.x > 1.0) orm *= texture2D(OrmTex, uv).rgb;
    else orm.b = 0.0;
#endif

    return clamp(orm, vec3(0.0, F0, 0.0), vec3(1.0, 1.0, 1.0));
}

vec2 GetParallaxCoord(const highp vec2 uv0, const highp vec3 v)
{
    highp vec2 uv = uv0;
#ifdef TERRA_NORMALMAP
    uv *= (1.0 + TexScale);
#endif
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP) && !defined(GL_ES)
    uv = uv - (vec2(v.x, -v.y) * (OffsetScale + 0.01) * texture2D(NormalTex, uv).a);
#endif
    return uv;
}


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 GetNormal(highp mat3 tbn, const vec2 uv, const vec2 uv1)
{
    // Retrieve the tangent space matrix
#ifndef HAS_TANGENTS
#if defined(TERRA_NORMALMAP)
    vec3 t = vec3(1.0, 0.0, 0.0);
    vec3 n = texture2D(TerraNormalTex, uv1.xy).xyz * 2.0 - 1.0;
    vec3 b = normalize(cross(n, t));
    t = normalize(cross(n ,b));
    tbn = mtxFromCols(t, b, n);
#elif defined(PAGED_GEOMETRY)
    const vec3 n = vec3(0.0, 1.0, 0.0);
    const vec3 t = vec3(1.0, 0.0, 0.0);
    vec3 b = normalize(cross(n, t));
    tbn = mtxFromCols(t, b, n);
#endif
#endif

#ifdef HAS_NORMALMAP
    if (TexSize1.x > 1.0) {
        vec3 normal = SurfaceSpecularColour.a * texture2D(NormalTex, uv.xy).xyz;
        return normalize(mul(tbn, (2.0 * normal - 1.0)));
    } else {
        return tbn[2];
    }
#else
    return tbn[2];
#endif
}

in highp vec3 vPosition;
in highp vec3 vPosition1;
#ifdef HAS_UV
in highp vec2 vUV0;
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
in mediump mat3 vTBN;
#else
in mediump vec3 vNormal;
#endif
#endif
#ifdef HAS_VERTEXCOLOR
in mediump vec3 vColor;
#endif
void main()
{
    v = normalize(CameraPosition - vPosition);
#ifdef HAS_UV
    vec2 uv = GetParallaxCoord(vUV0, v);
    vec2 uv1 = vUV0;
#else
    const vec2 uv = vec2(0.0, 0.0);
    const vec2 uv1 = vec2(0.0, 0.0);
#endif
#ifdef HAS_VERTEXCOLOR
    vec3 vertexColor = vColor;
#else
    const vec3 vertexColor = vec3(1.0, 1.0, 1.0);
#endif
    vec4 colour = GetAlbedo(uv, vertexColor);
    vec3 albedo = colour.rgb;
    float alpha = colour.a;
    vec3 orm = GetORM(uv, alpha);
    vec3 emission = GetEmission(uv);
    highp mat3 tbn;
#if !defined(TERRA_NORMALMAP) && !defined(PAGED_GEOMETRY)
#ifndef HAS_TANGENTS
    highp vec3 pos_dx = dFdx(vPosition);
    highp vec3 pos_dy = dFdy(vPosition);
    #ifdef HAS_UV
        highp vec3 tex_dx = max(dFdx(vec3(vUV0, 0.0)), vec3(0.001, 0.001, 0.001));
        highp vec3 tex_dy = max(dFdy(vec3(vUV0, 0.0)), vec3(0.001, 0.001, 0.001));
    #else
        highp vec3 tex_dx = vec3(0.001, 0.001, 0.001);
        highp vec3 tex_dy = vec3(0.001, 0.001, 0.001);
    #endif
    #ifdef HAS_NORMALS
        vec3 n = vNormal;
    #else
        vec3 n = cross(pos_dx, pos_dy);
    #endif
        highp vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
        t = normalize(t - n * dot(n, t));
        highp vec3 b = normalize(cross(n, t));
        tbn = mtxFromCols(t, b, n);
#else
    tbn = vTBN;
#endif // HAS_TANGENTS
#endif // !defined(TERRA_NORMALMAP) && !defined(PAGED_GEOMETRY)

    n = GetNormal(tbn, uv, uv1);
    float roughness = orm.g;
    float metallic = orm.b;
    float occlusion = orm.r;
    vec2 fragCoord = gl_FragCoord.xy * ViewportSize.zw;
    vec4 fragPos = mulMat4x4Float3(WorldViewProjMatrix, vPosition1);
    fragPos.xyz /= fragPos.w;
    fragDepth = gl_FragCoord.z / gl_FragCoord.w;
    float clampedDepth = (fragDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);
    vec4 fragPosPrev = mulMat4x4Float3(WorldViewProjPrev, vPosition1);
    fragPosPrev.xyz /= fragPosPrev.w;
    vec2 fragVelocity = (fragPosPrev.xy - fragPos.xy);
    float ao = 1.0;
#ifdef GL_ES
    // magic fix for GLES
    fragVelocity *= ViewportSize.zw;
#endif
#if defined(HAS_AO) || defined(HAS_SSR)
    vec2 nuv = fragCoord.xy + fragVelocity.xy;
    float dDepth = (clampedDepth - texture2D(OcclusionTex, nuv).g);
#endif
#ifdef HAS_AO
    if (dDepth <= 0.001) ao = texture2D(OcclusionTex, nuv).r;
#endif

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    vec3 diffuseColor = albedo * (1.0 - F0) * (1.0 - metallic);
    vec3 specularColor = mix(vec3(F0, F0, F0), albedo, metallic);
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    PBRInfo material;
#ifdef HAS_UV
    material.uv = vUV0;
#else
    material.uv = vec2(0.0, 0.0);
#endif
    material.NdotV = abs(dot(n, v)) + 0.001;
    material.diffuseColor = diffuseColor;
    material.specularColor = specularColor;
    material.metalness = metallic;
    material.occlusion = occlusion;
    material.ao = ao;
    material.perceptualRoughness = roughness;
    material.alphaRoughness = roughness * roughness;
    material.reflectance0 = specularColor.rgb;
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    material.reflectance90 = vec3(reflectance90, reflectance90, reflectance90);
    material.reflection = -normalize(reflect(v, n));

    vec3 color = vec3(0.0, 0.0, 0.0);

#if MAX_LIGHTS > 0
    color += EvaluateIBL(material);
#ifdef HAS_SSR
    vec3 ssr = vec3(0.0, 0.0, 0.0);
    if (dDepth <= 0.001) ssr = texture2D(SSR, nuv).rgb;
    if (ssr != vec3(0.0, 0.0, 0.0)) color = mix(color, ssr, metallic);
#endif

#if MAX_SHADOW_TEXTURES > 0
    highp vec3 lightSpacePosArray[MAX_SHADOW_TEXTURES];

    // Calculate the position of vertex attribute light space
    for (int i = 0; i < MAX_SHADOW_TEXTURES; ++i) {
        if (max(int(LightCount), 3) <= i) break;
        highp vec4 lightPos = mulMat4x4Float3(TexWorldViewProjMatrixArray[i], vPosition1);
        lightPos.xyz /= lightPos.w;
        lightSpacePosArray[i] = lightPos.xyz;
    }
#endif

#if MAX_SHADOW_TEXTURES > 0
    color += EvaluateDirectionalLight(material, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2]);
    color += EvaluateLocalLights(material, vPosition, lightSpacePosArray);
#else
    color += EvaluateDirectionalLight(material);
    color += EvaluateLocalLights(material, vPosition);
#endif

#else
    color += 3.0 * SurfaceAmbientColour.rgb * AmbientLightColour.rgb * albedo;
#endif // MAX_LIGHTS > 0
    color += emission;

#ifdef FORCE_FOG
#if MAX_LIGHTS > 0
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, v, LightDirectionArray[0].xyz, CameraPosition);
#else
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, v, vec3(0.0, 0.0, 0.0), CameraPosition);
#endif
#endif

    EvaluateBuffer(vec4(color, alpha), clampedDepth, mul(mat3(ViewMatrix), n), StaticObj * fragVelocity.xy);
}

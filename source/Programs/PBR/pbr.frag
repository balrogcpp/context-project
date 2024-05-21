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
uniform sampler2D OccTex;
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

uniform vec2 TexSize0;
uniform vec2 TexSize1;
uniform vec2 TexSize2;
uniform vec2 TexSize3;
uniform vec2 TexSize4;
uniform vec2 TexSize6;
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
uniform vec4 LightPointParamsArray[MAX_LIGHTS];
uniform vec4 LightSpotParamsArray[MAX_LIGHTS];
#endif // MAX_LIGHTS > 0
#if MAX_SHADOW_TEXTURES > 0
uniform highp vec4 PssmSplitPoints;
uniform highp mat4 TexWorldViewProjMatrixArray[MAX_SHADOW_TEXTURES];
uniform highp vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform float LightCastsShadowsArray[MAX_LIGHTS];
#endif // MAX_SHADOW_TEXTURES > 0
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

#if MAX_SHADOW_TEXTURES > 0 || defined(TERRA_NORMALMAP)
#include "pssm.glsl"
#endif

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct Light
{
    vec3 colorIntensity;  // rgb, pre-exposed intensity
    vec3 l;
    float attenuation;
    float NoL;
};

struct PixelParams
{
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float occlusion;
    float ssao;
    vec3 f0;            // full reflectance color (normal incidence angle)
    float f90;           // reflectance color at grazing angle
    float roughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 dfg;
    vec3 energyCompensation;
};

vec3 V, N;
vec2 UV;
float shading_NoV;

#if MAX_LIGHTS > 0
#include "ibl.glsl"
#include "lit.glsl"

float getDistanceAttenuation(const vec3 params, float distance)
{
    return 1.0 / (params.x + params.y * distance + params.z * distance * distance);
}

float getAngleAttenuation(const vec3 params, const vec3 lightDir, const vec3 toLight)
{
    float rho		= dot(-lightDir, toLight);
    float fSpotE	= saturate((rho - params.y) / (params.x - params.y));
    return pow(fSpotE, params.z);
}

vec3 EvaluateDirectionalLight(const PixelParams pixel, const highp vec3 pixelModelPosition)
{
    if (LightPositionArray[0].w != 0.0) return vec3(0.0, 0.0, 0.0);

    vec3 l = -LightDirectionArray[0].xyz; // Vector from surface point to light
    float NoL = saturate(dot(N, l));
    if (NoL <= 0.001) return vec3(0.0, 0.0, 0.0);
    float visibility = pixel.ssao;

#ifdef TERRA_LIGHTMAP
    visibility = FetchTerraShadow(UV);
    if (visibility < 0.001) return vec3(0.0, 0.0, 0.0);
#endif
#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
        float fDepth = gl_FragCoord.z / gl_FragCoord.w;

        for (int i = 0; i < PSSM_SPLITS; ++i) {
            if (fDepth <= PssmSplitPoints[i]) {
                highp vec4 lightSpacePos = mulMat4x4Half3(TexWorldViewProjMatrixArray[i], pixelModelPosition);
                lightSpacePos.xyz /= lightSpacePos.w;
                visibility *= CalcShadow(lightSpacePos.xyz, i);
                break;
            }
        }

        if (visibility < 0.001) return vec3(0.0, 0.0, 0.0);
    }
#endif

    Light light;
    light.NoL = NoL;
    light.colorIntensity = LightDiffuseScaledColourArray[0].xyz;
    visibility *= computeMicroShadowing(NoL, pixel.occlusion);
    light.attenuation = visibility;
    return surfaceShading(light, pixel);
}

vec3 EvaluateLocalLights(const PixelParams pixel, const highp vec3 pixelViewPosition, const highp vec3 pixelModelPosition)
{
    vec3 color = vec3(0.0, 0.0, 0.0);

#if MAX_SHADOW_TEXTURES > 0
    int PSSM_OFFSET = (LightPositionArray[0].w == 0.0) ? PSSM_SPLITS - 1 : 0;
#endif

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        if (LightPositionArray[i].w == 0.0) continue;

        highp vec3 l = (LightPositionArray[i].xyz - pixelViewPosition);
        float fLightD = length(l);
        l /= fLightD; // Vector from surface point to light

        if (fLightD > LightPointParamsArray[i].x) continue;

        float NoL = saturate(dot(N, l));
        if (NoL <= 0.001) continue;

        // attenuation is property of spot and point light
        float attenuation = getDistanceAttenuation(LightPointParamsArray[i].yzw, fLightD);
        if (attenuation < 0.001) continue;

        if(LightSpotParamsArray[i].w != 0.0) {
            attenuation *= getAngleAttenuation(LightSpotParamsArray[i].xyz, LightDirectionArray[i].xyz, l);
            if (attenuation < 0.001) continue;
        }

#if MAX_SHADOW_TEXTURES > 0
        if (LightCastsShadowsArray[i] != 0.0) {
            highp vec4 lightSpacePos = mulMat4x4Half3(TexWorldViewProjMatrixArray[i + PSSM_OFFSET], pixelModelPosition);
            lightSpacePos.xyz /= lightSpacePos.w;
            attenuation *= CalcShadow(lightSpacePos.xyz, i + PSSM_OFFSET);
            if (attenuation < 0.001) continue;
        }
#endif

        Light light;
        light.NoL = NoL;
        light.colorIntensity = LightDiffuseScaledColourArray[i].xyz;
        light.attenuation = attenuation;

        color += surfaceShading(light, pixel);
    }

    return color;
}


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 GetNormal(const highp mat3 tbn, const vec2 uv)
{
#ifdef HAS_NORMALMAP
#ifndef GL_ES
    // magic fix for GLES, uniform fails on old Blackbery phone, textureSize works
    if (TexSize1.x > 1.0) return normalize(mul(tbn, texture2D(NormalTex, uv.xy).xyz * 2.0 - 1.0));
#else
    if (textureSize(NormalTex, 0).x > 1) return normalize(mul(tbn, texture2D(NormalTex, uv.xy).xyz * 2.0 - 1.0));
#endif
    else return tbn[2];
#else
    return tbn[2];
#endif
}

vec3 GetNormal(const vec2 uv)
{
#ifndef HAS_TANGENTS
#if defined(TERRA_NORMALMAP)
    vec3 n = texture2D(TerraNormalTex, UV).xyz * 2.0 - 1.0;
    vec3 b = normalize(cross(n, vec3(1.0, 0.0, 0.0)));
    vec3 t = normalize(cross(n ,b));
    highp mat3 tbn = mtxFromCols(t, b, n);
    return GetNormal(tbn, uv);
#elif defined(PAGED_GEOMETRY)
    highp mat3 tbn = mtxFromCols(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0));
    return GetNormal(tbn, uv);
#endif
#endif

    return vec3(0.0, 0.0, 0.0);
}

vec3 GetORM(const vec2 uv, float spec)
{
#ifndef TERRA_NORMALMAP
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to material roughness
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r * saturate(1.0 - spec/128.0), 0.0);
#endif
#ifdef HAS_ORM
    if (TexSize2.x > 1.0) orm *= texture2D(OrmTex, uv).rgb;
    else orm.b = 0.0;
#endif

    return clamp(orm, vec3(0.0, MIN_PERCEPTUAL_ROUGHNESS, 0.0), vec3(1.0, 1.0, 1.0));
}

/**
 * Computes all the parameters required to shade the current pixel/fragment.
 * These parameters are derived from the MaterialInputs structure computed
 * by the user's material code.
 *
 * This function is also responsible for discarding the fragment when alpha
 * testing fails.
 */
void getPixelParams(const vec3 baseColor, const vec3 orm, float ssao, inout PixelParams pixel) {
    shading_NoV = clampNoV(dot(N, V));

    float roughness = orm.g;
    float metallic = orm.b;
    pixel.diffuseColor = computeDiffuseColor(baseColor, metallic);
    pixel.occlusion = orm.r;
    pixel.ssao = ssao;
    pixel.perceptualRoughness = roughness;
    pixel.roughness = perceptualRoughnessToRoughness(roughness);
    // Assumes an interface from air to an IOR of 1.5 for dielectrics
    const float reflectance = 0.5;
    float f0 = computeDielectricF0(reflectance);
    pixel.f0 = computeF0(baseColor, metallic, f0);

    pixel.dfg = EnvBRDFApprox(pixel.f0, pixel.perceptualRoughness, shading_NoV);

    // https://google.github.io/filament/Filament.md.html#toc5.6.2
    pixel.f90 = saturate(dot(pixel.f0, vec3(50.0 * 0.33, 50.0 * 0.33, 50.0 * 0.33)));

    // Energy compensation for multiple scattering in a microfacet model
    // See "Multiple-Scattering Microfacet BSDFs with the Smith Model"
    pixel.energyCompensation = 1.0 + pixel.f0 * (1.0 / pixel.dfg.y - 1.0);
}
#endif

#ifdef HAS_ALPHA
float computeMaskedAlpha(float a) {
    // Use derivatives to smooth alpha tested edges
    return (a - 0.5) / max(fwidth(a), 1e-3) + 0.5;
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
    if (albedo.a <= SurfaceAlphaRejection) {
        discard;
    }
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


#ifdef HAS_AO
#define KERNEL_RADIUS 3

// https://github.com/nvpro-samples/gl_ssao/blob/f6b010dc7a05346518cd13d3368d8d830a382ed9/bilateralblur.frag.glsl
float BlurFunction(const vec2 uv, float r, const float center_c, float center_d, inout float w_total)
{
    vec2 occ = texture2D(OccTex, uv).rg;
    float c = occ.r;
    float d = occ.g;

    const float sharpness = 1.0;
    const float BlurSigma = float(KERNEL_RADIUS) * 0.5;
    const float BlurFalloff = 1.0 / (2.0 * BlurSigma * BlurSigma);

    float ddiff = (d - center_d) * sharpness;
    float w = exp2(-r * r * BlurFalloff - ddiff * ddiff);
    w_total += w;

    return c * w;
}

float GetAO(const vec2 uv, float center_c, float center_d)
{
    float c_total = center_c;
    float w_total = 1.0;

    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv + TexSize4 * float(r), float(r), center_c, center_d, w_total);
    }
    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv - TexSize4 * float(r), float(r), center_c, center_d, w_total);
    }

    return c_total/w_total;
}
#endif

in highp vec3 vPosition;
in highp vec3 vPosition1;
#ifdef HAS_UV
in highp vec2 vUV0;
#endif
#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
in mediump mat3 vTBN;
#endif
#ifdef HAS_VERTEXCOLOR
in mediump vec3 vColor;
#endif
void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);

#ifdef HAS_UV
    vec2 uv = vUV0 * (1.0 + TexScale);
    vec2 uv1 = vUV0;
    UV = vUV0;
#else
    const vec2 uv = vec2(0.0, 0.0);
    const vec2 uv1 = vec2(0.0, 0.0);
#endif
#ifdef HAS_VERTEXCOLOR
    vec4 colour = GetAlbedo(uv, vColor);
#else
    vec4 colour = GetAlbedo(uv, vec3(1.0, 1.0, 1.0));
#endif

    vec3 albedo = colour.rgb;
    float alpha = colour.a;
    vec3 emission = GetEmission(uv);

    V = normalize(CameraPosition - vPosition);
    vec2 fragCoord = gl_FragCoord.xy * ViewportSize.zw;
    vec4 fragPos = mulMat4x4Half3(WorldViewProjMatrix, vPosition1);
    fragPos.xyz /= fragPos.w;
    highp float fragDepth = gl_FragCoord.z / gl_FragCoord.w;
    float clampedDepth = (fragDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);
    vec4 fragPosPrev = mulMat4x4Half3(WorldViewProjPrev, vPosition1);
    fragPosPrev.xyz /= fragPosPrev.w;
    vec2 fragVelocity = (fragPosPrev.xy - fragPos.xy);

#if MAX_LIGHTS > 0
    vec3 orm = GetORM(uv, alpha);

#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
    N = GetNormal(vTBN, uv);
#else
    N = GetNormal(uv);
#endif

    float ssao = 1.0;
#ifdef GL_ES
    // magic fix for GLES
    fragVelocity *= ViewportSize.zw;
#endif
#if defined(HAS_AO) || defined(HAS_SSR)
    vec2 nuv = fragCoord.xy + fragVelocity.xy;
    vec2 occ = texture2D(OccTex, nuv).rg;
    float dDepth = (clampedDepth - occ.g);
#endif
#ifdef HAS_AO
    if (dDepth <= 0.001) ssao = GetAO(nuv, occ.r, occ.g);
#endif

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    PixelParams pixel;
    getPixelParams(albedo, orm, ssao, pixel);

//#ifdef HAS_SSR
//    vec3 ssr = vec3(0.0, 0.0, 0.0);
//    if (dDepth <= 0.001) ssr = texture2D(SSR, nuv).rgb;
//    if (ssr != vec3(0.0, 0.0, 0.0)) color = mix(color, ssr, orm.b);
//#endif

    color += evaluateIBL(pixel);

#if MAX_SHADOW_TEXTURES > 0
    color += EvaluateDirectionalLight(pixel, vPosition1);
#endif
    color += EvaluateLocalLights(pixel, vPosition, vPosition1);

#else
    color += 3.0 * SurfaceAmbientColour.rgb * AmbientLightColour.rgb * albedo;
#endif // MAX_LIGHTS > 0

    color += emission;

#ifdef FORCE_FOG
#if MAX_LIGHTS > 0
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, V, LightDirectionArray[0].xyz, CameraPosition);
#else
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, V, vec3(0.0, 0.0, 0.0), CameraPosition);
#endif
#endif

    EvaluateBuffer(vec4(color, alpha), clampedDepth, mulMat3x3Float3(ViewMatrix, N), StaticObj * fragVelocity.xy);
}

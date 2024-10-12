// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "tonemap.glsl"
#include "math.glsl"
#include "fog.glsl"
#include "srgb.glsl"

#if defined(HAS_BASECOLORMAP)
uniform sampler2D AlbedoTex;
#endif
#if defined(HAS_NORMALMAP)
uniform sampler2D NormalTex;
#endif
#if defined(HAS_ORM)
uniform sampler2D OrmTex;
#endif
#if defined(HAS_EMISSIVEMAP)
uniform sampler2D EmissiveTex;
#endif
#if defined(HAS_AO)
uniform sampler2D OccTex;
#endif
#if MAX_SHADOW_TEXTURES > 0
#if defined(SHADOWMAP_ATLAS)
uniform sampler2D ShadowTex;
#else
uniform sampler2D ShadowTex0;
uniform sampler2D ShadowTex1;
uniform sampler2D ShadowTex2;
uniform sampler2D ShadowTex3;
#endif
#endif
#if defined(HAS_IBL)
uniform samplerCube SpecularEnvTex;
#endif
#if defined(TERRA_NORMALMAP)
uniform sampler2D TerraNormalTex;
#endif
#if defined(TERRA_LIGHTMAP)
uniform sampler2D TerraLightTex;
#endif

uniform vec2 TexelSize6;
#if !defined(SHADOWMAP_ATLAS)
uniform vec2 TexelSize7;
uniform vec2 TexelSize8;
uniform vec2 TexelSize9;
#endif
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform mat4 ViewMatrix;
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
    vec3  colorIntensity;  // rgb, pre-exposed intensity
    vec3  l;
    float attenuation;
    float NoL;
};

struct PixelParams
{
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float occlusion;
    float ssao;
    vec3  f0;            // full reflectance color (normal incidence angle)
    float roughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3  diffuseColor;            // color contribution from diffuse lighting
    vec3  dfg;
    vec3  energyCompensation;
#if defined(SHADING_MODEL_SUBSURFACE)
    float thickness;
    vec3  subsurfaceColor;
    float subsurfacePower;
#endif
#if defined(SHADING_MODEL_CLOTH) && defined(MATERIAL_HAS_SUBSURFACE_COLOR)
    vec3  subsurfaceColor;
#endif
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
    float visibility = 1.0;

#if defined(TERRA_LIGHTMAP)
    visibility = FetchTerraShadow(UV);
    if (visibility < 0.001) return vec3(0.0, 0.0, 0.0);
#endif
#if MAX_SHADOW_TEXTURES > 0
    if (LightCastsShadowsArray[0] != 0.0) {
        float fDepth = gl_FragCoord.z / gl_FragCoord.w;

        for (int i = 0; i < PSSM_SPLITS; ++i) {
            if (fDepth <= PssmSplitPoints[i]) {
                highp vec4 lightSpacePos = mulMat4x4Float3(TexWorldViewProjMatrixArray[i], pixelModelPosition);
                lightSpacePos.xyz /= lightSpacePos.w;
                visibility *= CalcShadow(lightSpacePos.xyz, i);
                break;
            }
        }

        if (visibility < 0.001) return vec3(0.0, 0.0, 0.0);
    }
#endif

    Light light;
    light.l = l;
    light.attenuation = 1.0;
    light.NoL = NoL;
    light.colorIntensity = LightDiffuseScaledColourArray[0].xyz;
    visibility *= computeMicroShadowing(NoL, pixel.occlusion);

    return surfaceShading(light, pixel, visibility);
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
        float visibility = 1.0;

        // attenuation is property of spot and point light
        float attenuation = getDistanceAttenuation(LightPointParamsArray[i].yzw, fLightD);
        if (attenuation < 0.001) continue;

        if(LightSpotParamsArray[i].w != 0.0) {
            attenuation *= getAngleAttenuation(LightSpotParamsArray[i].xyz, LightDirectionArray[i].xyz, l);
            if (attenuation < 0.001) continue;
        }

#if MAX_SHADOW_TEXTURES > 0
        if (LightCastsShadowsArray[i] != 0.0) {
            highp vec4 lightSpacePos = mulMat4x4Float3(TexWorldViewProjMatrixArray[i + PSSM_OFFSET], pixelModelPosition);
            lightSpacePos.xyz /= lightSpacePos.w;
            visibility *= CalcShadow(lightSpacePos.xyz, i + PSSM_OFFSET);
            if (visibility < 0.001) continue;
        }
#endif

        Light light;
        light.l = l;
        light.attenuation = attenuation;
        light.NoL = NoL;
        light.colorIntensity = LightDiffuseScaledColourArray[i].xyz;

        color += surfaceShading(light, pixel, visibility);
    }

    return color;
}


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 GetNormal(const highp mat3 tbn, const vec2 uv)
{
#if defined(HAS_NORMALMAP)
    return normalize(mul(tbn, texture(NormalTex, uv.xy).xyz * 2.0 - 1.0));
#else
    return tbn[2];
#endif
}

mat3 GetTBN(const vec2 uv, const vec3 position)
{
#if !defined(HAS_TANGENTS)
#if defined(TERRA_NORMALMAP)
    vec3 n = texture(TerraNormalTex, UV).xyz * 2.0 - 1.0;
    vec3 b = normalize(cross(n, vec3(1.0, 0.0, 0.0)));
    vec3 t = normalize(cross(n ,b));
    highp mat3 tbn = mtxFromCols(t, b, n);
    return tbn;
#elif defined(PAGED_GEOMETRY)
    highp mat3 tbn = mtxFromCols(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0));
    return tbn;
#else

    highp vec3 pos_dx = dFdx(position);
    highp vec3 pos_dy = dFdy(position);
#if defined(HAS_UV)
    highp vec3 tex_dx = max(dFdx(vec3(uv, 0.0)), vec3(0.001, 0.001, 0.001));
    highp vec3 tex_dy = max(dFdy(vec3(uv, 0.0)), vec3(0.001, 0.001, 0.001));
#else
    const highp vec3 tex_dx = vec3(0.001, 0.001, 0.001);
    const highp vec3 tex_dy = vec3(0.001, 0.001, 0.001);
#endif
#if defined(HAS_NORMALS)
    vec3 n = vNormal;
#else
    vec3 n = cross(pos_dx, pos_dy);
#endif
    highp vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
    t = normalize(t - n * dot(n, t));
    highp vec3 b = normalize(cross(n, t));
    tbn = mtxFromCols(t, b, n);
    return tbn;
#endif
#endif

    return mat3(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));
}

vec3 GetORM(const vec2 uv, float spec)
{
#if !defined(TERRA_NORMALMAP)
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to material roughness
    vec3 orm = vec3(SurfaceShininessColour, SurfaceSpecularColour.r * (1.0 - spec), 0.0);
#endif
#if defined(HAS_ORM)
    orm *= texture(OrmTex, uv).rgb;
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

    float roughness = orm.y;
    float metallic = orm.z;
    pixel.diffuseColor = computeDiffuseColor(baseColor, metallic);
    pixel.occlusion = orm.x;
    pixel.ssao = ssao;
    pixel.perceptualRoughness = roughness;
    pixel.roughness = perceptualRoughnessToRoughness(roughness);
    // Assumes an interface from air to an IOR of 1.5 for dielectrics
    //const float reflectance = 0.5;
    //float f0 = computeDielectricF0(reflectance);
    pixel.f0 = computeF0(baseColor, metallic, 0.04); // f0 = 0.04

    pixel.dfg = EnvBRDFApprox(pixel.perceptualRoughness, shading_NoV);

#if !defined(SHADING_MODEL_CLOTH)
// Energy compensation for multiple scattering in a microfacet model
// See "Multiple-Scattering Microfacet BSDFs with the Smith Model"
    pixel.energyCompensation = 1.0 + pixel.f0 * (1.0 / pixel.dfg.y - 1.0);
#else
    pixel.energyCompensation = vec3(1.0, 1.0, 1.0);
#endif
}
#endif

#if defined(HAS_ALPHA)
float computeMaskedAlpha(const float a) {
    // Use derivatives to smooth alpha tested edges
    return (a - 0.5) / max(fwidth(a), 1e-3) + 0.5;
}

void applyAlphaMask(inout vec4 baseColor) {
    baseColor.a = computeMaskedAlpha(baseColor.a);
    if (baseColor.a <= 0.0) {
        discard;
    }
}
#endif

// Sampler helper functions
vec4 GetAlbedo(const vec2 uv, const vec3 color)
{
    vec4 albedo = vec4(SurfaceDiffuseColour.rgb * color, 1.0);
#if defined(HAS_BASECOLORMAP)
    albedo *= texture(AlbedoTex, uv);
#endif

#if defined(HAS_ALPHA)
    if (albedo.a <= SurfaceAlphaRejection) {
        discard;
    }
#endif
    return vec4(SRGBtoLINEAR(albedo.rgb), albedo.a);
}

vec3 GetEmission(const vec2 uv)
{
    vec3 emission = SurfaceEmissiveColour.rgb;
#if defined(HAS_EMISSIVEMAP)
    emission += texture(EmissiveTex, uv).rgb;
#endif
    return SRGBtoLINEAR(emission) * SurfaceSpecularColour.b;
}


in highp vec3 vPosition;
in highp vec3 vPosition1;
in mediump vec4 vPrevScreenPosition;
#if defined(HAS_UV)
in highp vec2 vUV0;
#endif
#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
in mediump mat3 vTBN;
#endif
#if defined(HAS_VERTEXCOLOR)
in mediump vec3 vColor;
#endif
out vec4 FragColor;
void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);

#if defined(HAS_UV)
    vec2 uv = vUV0;
    UV = vUV0;
#if defined(TERRA_LIGHTMAP)
    uv *= TexScale;
#endif
#else
    const vec2 uv = vec2(0.0, 0.0);
#endif
#if defined(HAS_VERTEXCOLOR)
    vec4 colour = GetAlbedo(uv, vColor);
#else
    vec4 colour = GetAlbedo(uv, vec3(1.0, 1.0, 1.0));
#endif

    vec3 albedo = colour.rgb;
    float alpha = colour.a;
    vec3 emission = GetEmission(uv);

    V = normalize(CameraPosition - vPosition);
    vec2 fragCoord = gl_FragCoord.xy * ViewportSize.zw;
    vec2 fragVelocity = (vPrevScreenPosition.xy / vPrevScreenPosition.w) - fragCoord;
    float fragDepth = gl_FragCoord.z / gl_FragCoord.w;
    float clampedDepth = (fragDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);

#if MAX_LIGHTS > 0
    vec3 orm = GetORM(uv, alpha);

#if defined(HAS_NORMALS) && defined(HAS_TANGENTS)
    N = GetNormal(vTBN, uv);
#else
    highp mat3 tbn = GetTBN(uv, vPosition);
    N = GetNormal(tbn, uv);
#endif

    float ssao = 1.0;

#if defined(HAS_AO)
    vec2 nuv = fragCoord + fragVelocity;
    vec2 occ = textureLod(OccTex, fragCoord, 0.0).rg;
//    float dDepth = (clampedDepth - occ.g);
//    ssao = GetAO(fragCoord, occ.r, occ.g);
    ssao = occ.r;
#endif

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    PixelParams pixel;
    getPixelParams(albedo, orm, ssao, pixel);

    color += evaluateIBL(pixel);
    color += EvaluateDirectionalLight(pixel, vPosition1);
    color += EvaluateLocalLights(pixel, vPosition, vPosition1);

#else
    color += 3.0 * SurfaceAmbientColour.rgb * AmbientLightColour.rgb * albedo;
#endif // MAX_LIGHTS > 0

    color += emission;

#if defined(FORCE_FOG)
#if MAX_LIGHTS > 0
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, V, LightDirectionArray[0].xyz, CameraPosition);
#else
    color = ApplyFog(color, FogParams.x, FogColour.rgb, fragDepth, V, vec3(0.0, 0.0, 0.0), CameraPosition);
#endif
#endif

    FragColor = SafeHDR(vec4(tonemap(color), alpha));
}
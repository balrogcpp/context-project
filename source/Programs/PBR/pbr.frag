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
#define USE_TEX_LOD
#define SPHERICAL_HARMONICS_BANDS 8
#else
#define SPHERICAL_HARMONICS_BANDS 3
#endif

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
mediump vec3 Diffuse(const mediump vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
mediump vec3 SpecularReflection(const mediump vec3 reflectance0, const mediump vec3 reflectance90, const mediump float VdotH)
{
    return reflectance0 + (reflectance90 - reflectance0) * pow5(1.0 - VdotH);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
mediump float GeometricOcclusion(const mediump float NdotL, const mediump float NdotV, const mediump float r)
{
    mediump float r2 = (r * r);
    mediump float r3 = (1.0 - r2);
    mediump float attenuationL = (2.0 * NdotL) / (NdotL + sqrt(r2 + r3 * (NdotL * NdotL)));
    mediump float attenuationV = (2.0 * NdotV) / (NdotV + sqrt(r2 + r3 * (NdotV * NdotV)));
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

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
mediump vec3 EnvBRDFApprox(const mediump vec3 specularColor, const mediump float roughness, const mediump float NdotV)
{
    const mediump vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const mediump vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    mediump vec4 r = roughness * c0 + c1;
    mediump float a004 = min(r.x * r.x, exp2( -9.28 * NdotV )) * r.x + r.y;
    mediump vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return specularColor * AB.x + AB.y;
}

// https://google.github.io/filament/Filament.html 5.4.3.1 Diffuse BRDF integration
mediump vec3 Irradiance_SphericalHarmonics(const mediump vec3 iblSH[9], const mediump vec3 n) {
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

vec3 Irradiance_RoughnessOne(const samplerCube SpecularEnvTex, const mediump vec3 n) {
    // note: lod used is always integer, hopefully the hardware skips tri-linear filtering
    return textureCubeLod(SpecularEnvTex, n, 9.0).rgb;
}

#define NUM_TEXTURES 0
#ifdef HAS_BASECOLORMAP
uniform sampler2D AlbedoTex;
#define NUM_TEXTURES 1
#endif // HAS_BASECOLORMAP
#ifdef HAS_NORMALMAP
uniform sampler2D NormalTex;
#define NUM_TEXTURES 2
#endif // HAS_NORMALMAP
#ifdef HAS_ORM
uniform sampler2D OrmTex;
#define NUM_TEXTURES 3
#endif // HAS_ORM
#ifdef HAS_EMISSIVEMAP
uniform sampler2D EmissiveTex;
#define NUM_TEXTURES 4
#endif // HAS_EMISSIVEMAP
#ifdef HAS_IBL
uniform samplerCube SpecularEnvTex;
#define NUM_TEXTURES 5
#endif // HAS_IBL
#ifdef TERRA_NORMALMAP
uniform sampler2D TerraNormalTex;
#define NUM_TEXTURES 3
#endif // TERRA_NORMALMAP
#ifdef TERRA_LIGHTMAP
uniform sampler2D TerraLightTex;
#define NUM_TEXTURES 4
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

uniform mediump vec3 iblSH[9];
#ifdef TERRA_LIGHTMAP
uniform mediump vec2 InvTerraLightTexSize;
#endif
uniform highp mat4 ViewMatrix;
uniform highp vec3 CameraPosition;
uniform highp vec4 Time;
uniform mediump float LightCount;
#if MAX_LIGHTS > 0
uniform highp vec4 LightPositionArray[MAX_LIGHTS];
uniform mediump vec4 LightDirectionArray[MAX_LIGHTS];
uniform mediump vec4 LightDiffuseScaledColourArray[MAX_LIGHTS];
uniform mediump vec4 LightAttenuationArray[MAX_LIGHTS];
uniform mediump vec4 LightSpotParamsArray[MAX_LIGHTS];
#endif // MAX_LIGHTS > 0
uniform mediump vec4 AmbientLightColour;
uniform mediump vec4 SurfaceAmbientColour;
uniform mediump vec4 SurfaceDiffuseColour;
uniform mediump vec4 SurfaceSpecularColour;
uniform mediump vec4 SurfaceEmissiveColour;
uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
uniform highp float FarClipDistance;
uniform highp float NearClipDistance;
uniform mediump float TexScale;
#ifdef HAS_NORMALMAP
#ifdef HAS_PARALLAXMAP
uniform mediump float OffsetScale;
#endif // HAS_PARALLAXMAP
#endif // HAS_NORMALMAP
#if MAX_SHADOW_TEXTURES > 0
uniform mediump vec4 ShadowDepthRangeArray[MAX_SHADOW_TEXTURES];
uniform mediump float LightCastsShadowsArray[MAX_LIGHTS];
uniform mediump vec4 PssmSplitPoints;
uniform mediump vec4 ShadowColour;
#if MAX_SHADOW_TEXTURES > 0
uniform mediump vec2 ShadowTexel0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform mediump vec2 ShadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform mediump vec2 ShadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform mediump vec2 ShadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform mediump vec2 ShadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform mediump vec2 ShadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform mediump vec2 ShadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform mediump vec2 ShadowTexel7;
#endif
#endif // MAX_SHADOW_TEXTURES > 0

#if MAX_SHADOW_TEXTURES > 0
#include "pssm.glsl"
#endif


#ifdef HAS_IBL
mediump vec3 DiffuseIrradiance(const mediump vec3 n)
{
    if (iblSH[0].r >= HALF_MAX_MINUS1) {
        return LINEARtoSRGB(textureCubeLod(SpecularEnvTex, n, 9.0).rgb);
    } else {
        return Irradiance_SphericalHarmonics(iblSH, n);
    }
}

mediump vec3 GetIblSpeculaColor(const mediump vec3 reflection, const mediump float perceptualRoughness)
{
#ifdef USE_TEX_LOD
    return LINEARtoSRGB(LINEARtoSRGB(textureCubeLod(SpecularEnvTex, reflection, perceptualRoughness * 9.0).rgb));
#else
    return LINEARtoSRGB(textureCube(SpecularEnvTex, reflection).rgb);
#endif
}
#endif // HAS_IBL


// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Texs are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Texs [3] for additional discussion.
mediump vec3 GetIBL(const mediump vec3 diffuseColor, const mediump vec3 specularColor, const mediump float perceptualRoughness, const mediump float NdotV, const mediump vec3 n, const mediump vec3 reflection)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    mediump vec3 brdf = EnvBRDFApprox(specularColor, perceptualRoughness, NdotV);

#ifdef HAS_IBL
    mediump vec3 diffuseLight = DiffuseIrradiance(reflection);
    mediump vec3 specularLight = GetIblSpeculaColor(reflection, perceptualRoughness);
#else
    mediump vec3 diffuseLight = Irradiance_SphericalHarmonics(iblSH, reflection);
    mediump vec3 specularLight = diffuseLight;
#endif

    mediump vec3 diffuse = (diffuseLight * diffuseColor);
    mediump vec3 specular = specularLight * ((specularColor * brdf.x) + brdf.y);

    return diffuse + specular;
}


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
highp vec3 GetNormal(const highp vec3 normal, highp mat3 tbn, const mediump vec2 uv1)
{
#ifdef TERRA_NORMALMAP
    highp vec3 t = vec3(1.0, 0.0, 0.0);
    highp vec3 ng = texture2D(TerraNormalTex, uv1).xyz * 2.0 - 1.0;
    highp vec3 b = normalize(cross(ng, t));
    t = normalize(cross(ng ,b));
    tbn = mtxFromCols3x3(t, b, ng);

#ifdef HAS_NORMALMAP
    highp vec3 n = SurfaceSpecularColour.a * normal;
    return normalize(mul(tbn, (2.0 * n - 1.0)));
#else
    return tbn[2].xyz;
#endif // HAS_NORMALMAP
    #else

#ifdef HAS_NORMALMAP
    highp vec3 n = SurfaceSpecularColour.a * normal;
    return normalize(mul(tbn, ((2.0 * n - 1.0))));
#else
    return tbn[2].xyz;
#endif // HAS_NORMALMAP
#endif // TERRA_NORMALMAP
}


// Sampler helper functions
void GetAlbedo(inout mediump vec4 color, const mediump vec4 vertexColor)
{
#ifdef HAS_BASECOLORMAP
    color *= vertexColor;
#else
    color = vertexColor;
#endif
    color *= SurfaceDiffuseColour;
    color.rgb = SRGBtoLINEAR(color.rgb);
}

void GetEmission(inout mediump vec3 emission)
{
#ifdef HAS_EMISSIVEMAP
    emission = SRGBtoLINEAR(SurfaceEmissiveColour.rgb) + SRGBtoLINEAR(SurfaceSpecularColour.b * emission);
#else
    emission = SRGBtoLINEAR(SurfaceEmissiveColour.rgb);
#endif
}

void GetORM(inout mediump vec3 ORM, const mediump float spec)
{
#ifdef HAS_ORM
    ORM *= vec3(1.0, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#else
    ORM = vec3(1.0, SurfaceSpecularColour.r, SurfaceSpecularColour.g);
#endif

#ifdef TERRA_NORMALMAP
    //https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
    // converting phong specular value to pbr roughness
    ORM.g *= (1.0 - 0.25 * pow(spec, 0.2));
    ORM.b = 0.0;
#endif

    ORM = clamp(ORM, vec3(0.0, F0, 0.0), vec3(1.0, 1.0, 1.0));
}

in highp vec3 vWorldPosition;
in highp mat3 vTBN;
in highp vec2 vUV0;
in mediump vec4 vColor;
in mediump vec4 vScreenPosition;
in mediump vec4 vPrevScreenPosition;
#if MAX_SHADOW_TEXTURES > 0
in highp vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
void main()
{
    highp vec2 uv = vUV0.xy * (1.0 + TexScale);
#if defined(HAS_NORMALMAP) && defined(HAS_PARALLAXMAP)
    highp vec3 v = normalize(CameraPosition - vWorldPosition);
    uv -= (vec2(v.x, -v.y) * (OffsetScale * texture2D(NormalTex, uv).a));
#endif

    mediump vec4 colour;
#ifdef HAS_BASECOLORMAP
    colour = texture2D(AlbedoTex, uv);
#ifdef HAS_ALPHA
    if (colour.a < 0.5) discard;
#endif
#endif

    mediump vec3 ORM;
#ifdef HAS_ORM
    ORM = texture2D(OrmTex, uv).rgb;
#endif

    mediump vec3 emission;
#ifdef HAS_EMISSIVEMAP
    emission = texture2D(EmissiveTex, uv).xyz;
#endif

    highp vec3 normal;
#ifdef HAS_NORMALMAP
    normal = texture2D(NormalTex, uv).xyz;
#endif

    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
    FragData[MRT_VELOCITY] = vec4((vScreenPosition.xz / vScreenPosition.w) - (vPrevScreenPosition.xz / vPrevScreenPosition.w), 0.0, 0.0);

#ifdef CHECKERBOARD
    if (ExcludePixel()) return;
#endif

    GetAlbedo(colour, vColor);
    mediump vec3 albedo = colour.rgb;
    mediump float alpha = colour.a;

    GetEmission(emission);

    GetORM(ORM, alpha);
    mediump float roughness = ORM.g;
    mediump float metallic = ORM.b;
    mediump float occlusion = ORM.r;
    FragData[MRT_GLOSS] = vec4(metallic, roughness, alpha, 0.0);

    highp vec3 n = GetNormal(normal, vTBN, vUV0.xy);
    FragData[MRT_NORMALS] = vec4(normalize(mul(ViewMatrix, vec4(n, 0.0)).xyz), 1.0);

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    mediump vec3 diffuseColor = albedo * ((1.0 - F0) * (1.0 - metallic));
    mediump vec3 specularColor = mix(vec3(F0, F0, F0), albedo, metallic);

    // Compute reflectance.
    mediump float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    mediump vec3 reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    mediump vec3 reflectance0 = specularColor.rgb;

#if MAX_SHADOW_TEXTURES > 0
    int texCounter = 0;
#endif
#if !defined(HAS_PARALLAXMAP)
    highp vec3 v = normalize(CameraPosition - vWorldPosition);
#endif
    mediump float NdotV = clamp(dot(n, v), 0.001, 1.0);
    mediump vec3 color = vec3(0.0, 0.0, 0.0);
#if MAX_LIGHTS > 0
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        highp vec3 l = -normalize(LightDirectionArray[i].xyz); // Vector from surface point to light
        highp vec3 h = normalize(l + v); // Half vector between both l and v
        highp float NdotL = clamp(dot(n, l), 0.001, 1.0);

        // attenuation is property of spot and point light
        highp vec4 vLightPositionArray = LightPositionArray[i];
        mediump float attenuation = 1.0;
        if (vLightPositionArray.w != 0.0) {
            mediump vec4 vAttParams = LightAttenuationArray[i];
            highp float range = vAttParams.x;
            highp vec3 vLightViewH = vLightPositionArray.xyz - vWorldPosition;
            highp float fLightD = length(vLightViewH);
            highp float fLightD2 = fLightD * fLightD;
            highp vec3 vLightView = normalize(vLightViewH);
            mediump float attenuation_const = vAttParams.y;
            mediump float attenuation_linear = vAttParams.z;
            mediump float attenuation_quad = vAttParams.w;
            attenuation = biggerhp(range, fLightD) / (attenuation_const + (attenuation_linear * fLightD) + (attenuation_quad * fLightD2));

            // spotlight
            mediump vec4 vSpotParams = LightSpotParamsArray[i];
            if (vSpotParams.w != 0.0) {
                mediump float outerRadius = vSpotParams.z;
                mediump float fallof = vSpotParams.x;
                mediump float innerRadius = vSpotParams.y;

                mediump float rho = dot(l, vLightView);
                mediump float fSpotE = clamp((rho - innerRadius) / (fallof - innerRadius), 0.0, 1.0);
                attenuation *= pow(fSpotE, outerRadius);
            }
        }

        mediump float light = NdotL * attenuation;
        highp float alphaRoughness = roughness * roughness;
        highp float NdotH = clamp(dot(n, h), 0.0, 1.0);
        mediump float LdotH = clamp(dot(l, h), 0.0, 1.0);
        mediump float VdotH = clamp(dot(v, h), 0.0, 1.0);

        // Calculate the shading terms for the microfacet specular shading model
        mediump vec3 F = SpecularReflection(reflectance0, reflectance90, VdotH);
        mediump vec3 diffuseContrib = (1.0 - F) * Diffuse(diffuseColor);

        // Calculation of analytical lighting contribution
        mediump float G = GeometricOcclusion(NdotL, NdotV, alphaRoughness);
        mediump float D = MicrofacetDistribution(alphaRoughness, NdotH);
        mediump vec3 specContrib = (F * (G * D)) / (4.0 * (NdotL * NdotV));

#ifdef TERRA_LIGHTMAP
        if (i == 0) {
            light *= FetchTerraShadow(TerraLightTex, vUV0.xy, InvTerraLightTexSize);
        }
#endif

#if MAX_SHADOW_TEXTURES > 0
        if (LightCastsShadowsArray[i] != 0.0) {
            light *= clamp(GetShadow(vLightSpacePosArray, vScreenPosition.z, i, texCounter) + ShadowColour.r, 0.0, 1.0);
        }
#endif

        color += LightDiffuseScaledColourArray[i].xyz * (light * (diffuseContrib + specContrib));
    } //  lightning loop
#endif //  MAX_LIGHTS > 0

// Calculate lighting contribution from image based lighting source (IBL)
    mediump vec3 ambient = occlusion * (SurfaceAmbientColour.rgb * (AmbientLightColour.rgb * GetIBL(diffuseColor, specularColor, roughness, NdotV, n, -normalize(reflect(v, n)))));

// Apply optional PBR terms for additional (optional) shading
    color += ambient;
    color *= occlusion;
    color += emission;

    color = ApplyFog(color, FogParams, FogColour.rgb, vScreenPosition.z);

    FragData[MRT_COLOR] = vec4(SafeHDR(color), alpha);
}

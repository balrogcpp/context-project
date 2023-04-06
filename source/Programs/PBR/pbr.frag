// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif


#define USE_MRT
#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"
#ifdef SHADOWRECEIVER
#include "receiver.glsl"
#endif


// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Diffuse(const vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}


// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
//----------------------------------------------------------------------------------------------------------------------
mediump vec3 SpecularReflection(const mediump vec3 reflectance0, const mediump vec3 reflectance90, const mediump float VdotH)
{
    return reflectance0 + (reflectance90 - reflectance0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}


// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
//----------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------
highp float MicrofacetDistribution(const highp float alphaRoughness, const highp float NdotH)
{
    highp float roughnessSq = alphaRoughness * alphaRoughness;
    highp float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
    return roughnessSq / (M_PI * (f * f));
}


#ifdef USE_IBL
// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
//----------------------------------------------------------------------------------------------------------------------
mediump vec3 envBRDFApprox(const mediump vec3 specularColor, const mediump float roughness, const mediump float NdotV)
{
    const mediump vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const mediump vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    const mediump vec4 r = roughness * c0 + c1;
    mediump float a004 = min(r.x * r.x, exp2( -9.28 * NdotV )) * r.x + r.y;
    mediump vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return specularColor * AB.x + AB.y;
}


// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
mediump vec3 GetIBLContribution(const sampler2D BrdfLUT, const samplerCube DiffuseEnvMap, const samplerCube SpecularEnvMap, const mediump vec3 diffuseColor, const mediump vec3 specularColor, const mediump float perceptualRoughness, const mediump float NdotV, const mediump vec3 n, const mediump vec3 reflection)
{
    // retrieve a scale and bias to F0. See [1], Figure 3
    mediump vec3 brdf = envBRDFApprox(specularColor, perceptualRoughness, NdotV);
    mediump vec3 diffuseLight = SRGBtoLINEAR(textureCube(DiffuseEnvMap, n)).rgb;

#ifdef USE_TEX_LOD
    mediump vec3 specularLight = SRGBtoLINEAR(textureCubeLod(SpecularEnvMap, reflection, perceptualRoughness * 9.0)).rgb;
#else
    mediump vec3 specularLight = SRGBtoLINEAR(textureCube(SpecularEnvMap, reflection)).rgb;
#endif // USE_TEX_LOD

    mediump vec3 diffuse = (diffuseLight * diffuseColor);
    mediump vec3 specular = specularLight * ((specularColor * brdf.x) + brdf.y);

    return diffuse + specular;
}
#endif // USE_IBL


// varyings
in highp vec3 vPosition;
in highp vec2 vUV0;
in float vDepth;
in vec4 vColor;
in vec4 vScreenPosition;
in vec4 vPrevScreenPosition;
in mat3 vTBN;

// uniforms
#ifdef HAS_BASECOLORMAP
#define HAS_ALPHA
uniform sampler2D AlbedoMap;
#endif // HAS_BASECOLORMAP
#ifdef HAS_NORMALMAP
uniform sampler2D NormalMap;
#endif // HAS_NORMALMAP
#ifdef HAS_ORM
uniform sampler2D OrmMap;
#endif // HAS_ORM
#ifdef HAS_EMISSIVEMAP
uniform sampler2D EmissiveMap;
#endif // HAS_EMISSIVEMAP
#ifdef USE_IBL
uniform samplerCube DiffuseEnvMap;
uniform samplerCube SpecularEnvMap;
uniform sampler2D BrdfLUT;
#endif // USE_IBL

// lights
uniform highp vec3 CameraPosition;
uniform highp vec4 Time;
uniform lowp float LightCount;
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
uniform vec4 SurfaceShininessColour;
uniform vec4 SurfaceEmissiveColour;
uniform float SurfaceAlphaRejection;
uniform float FarClipDistance;
uniform float NearClipDistance;
uniform float FrameTime;
uniform float TexScale;
uniform float TexLod;
#ifdef HAS_NORMALMAP
#define HAS_PARALLAXMAP
uniform float OffsetScale;
#endif // HAS_NORMALMAP

// shadow receiver
#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
in vec4 vLightSpacePosArray[MAX_SHADOW_TEXTURES];
uniform lowp float LightCastsShadowsArray[MAX_LIGHTS];
uniform sampler2D ShadowMap0;
uniform vec2 ShadowTexel0;
#if MAX_SHADOW_TEXTURES > 1
uniform sampler2D ShadowMap1;
uniform vec2 ShadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform sampler2D ShadowMap2;
uniform vec2 ShadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform sampler2D ShadowMap3;
uniform vec2 ShadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform sampler2D ShadowMap4;
uniform vec2 ShadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform sampler2D ShadowMap5;
uniform vec2 ShadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform sampler2D ShadowMap6;
uniform vec2 ShadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform sampler2D ShadowMap7;
uniform vec2 ShadowTexel7;
#endif
uniform vec4 PssmSplitPoints;
uniform float ShadowFilterSize;
uniform int ShadowFilterIterations;
#endif // MAX_SHADOW_TEXTURES > 0
#endif // SHADOWRECEIVER


// helper functions
#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
float GetShadow(const int counter)
{
    if (vDepth >= PssmSplitPoints.w)
        return 1.0;
#if MAX_SHADOW_TEXTURES > 0
    else if (counter == 0)
        return CalcDepthShadow(ShadowMap0, vLightSpacePosArray[0], ShadowTexel0*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (counter == 1)
        return CalcDepthShadow(ShadowMap1, vLightSpacePosArray[1], ShadowTexel1*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (counter == 2)
        return CalcDepthShadow(ShadowMap2, vLightSpacePosArray[2], ShadowTexel2*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (counter == 3)
        return CalcDepthShadow(ShadowMap3, vLightSpacePosArray[3], ShadowTexel3*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (counter == 4)
        return CalcDepthShadow(ShadowMap4, vLightSpacePosArray[4], ShadowTexel4*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (counter == 5)
        return CalcDepthShadow(ShadowMap5, vLightSpacePosArray[5], ShadowTexel5*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (counter == 6)
        return CalcDepthShadow(ShadowMap6, vLightSpacePosArray[6], ShadowTexel6*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (counter == 7)
        return CalcDepthShadow(ShadowMap7, vLightSpacePosArray[7], ShadowTexel7*ShadowFilterSize, ShadowFilterIterations);
#endif

    return 1.0;
}


//----------------------------------------------------------------------------------------------------------------------
float CalcPSSMDepthShadow(const vec4 PssmSplitPoints, const vec4 lightSpacePos0, const vec4 lightSpacePos1, const vec4 lightSpacePos2, const sampler2D ShadowMap0, const sampler2D ShadowMap1, const sampler2D ShadowMap2)
{
    // calculate shadow
    if (vDepth <= PssmSplitPoints.x)
        return CalcDepthShadow(ShadowMap0, lightSpacePos0, ShadowTexel0*ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.y)
        return CalcDepthShadow(ShadowMap1, lightSpacePos1, ShadowTexel1*ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= PssmSplitPoints.z)
        return CalcDepthShadow(ShadowMap2, lightSpacePos2, ShadowTexel2*ShadowFilterSize, ShadowFilterIterations);
    else
        return 1.0;
}


//----------------------------------------------------------------------------------------------------------------------
float GetPSSMShadow(const int number)
{
    //  special case, as only light #0 can be with pssm shadows
    if (LightAttenuationArray[0].x > 10000.0)
    {
        if (number == 0)
            return CalcPSSMDepthShadow(PssmSplitPoints, \
                                    vLightSpacePosArray[0], vLightSpacePosArray[1], vLightSpacePosArray[2], \
                                    ShadowMap0, ShadowMap1, ShadowMap2);
        else
            return GetShadow(number + 2);
    } else {
        return GetShadow(number);
    }
}
#endif // MAX_SHADOW_TEXTURES > 0
#endif // SHADOWRECEIVER


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
//----------------------------------------------------------------------------------------------------------------------
highp vec3 GetNormal(const vec2 uv)
{
#ifndef HAS_NORMALS
#ifndef HAS_TANGENTS
    highp vec3 n0 = cross(dFdx(vPosition), dFdy(vPosition));

#ifdef HAS_NORMALMAP
    highp vec3 n1 = texture2D(NormalMap, uv, TexLod).xyz;
    vec3 b = normalize(cross(n0, vec3(1.0, 0.0, 0.0)));
    vec3 t = normalize(cross(n0, b));
    n1 = normalize(mat3(t, b, n0) * ((2.0 * n1 - 1.0)));
    return n1;
#else
    return n0;
#endif // HAS_NORMALMAP
#endif // !HAS_TANGENTS
#endif // !HAS_NORMALS

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
#ifdef HAS_NORMALMAP
    highp vec3 n = texture2D(NormalMap, uv, TexLod).xyz;
    n = normalize(vTBN * ((2.0 * n - 1.0)));
    return n;
#else
    highp vec3 n = vTBN[2].xyz;
    return n;
#endif // HAS_NORMALMAP
#endif // HAS_TANGENTS
#endif // HAS_NORMALS
}


// Sampler helper functions
//----------------------------------------------------------------------------------------------------------------------
vec4 GetAlbedo(const vec2 uv)
{
    vec4 albedo = SurfaceDiffuseColour.rgba * vColor;

#ifdef HAS_BASECOLORMAP
    albedo *= texture2D(AlbedoMap, uv, TexLod);
#endif
    return SRGBtoLINEAR(albedo);
}


//----------------------------------------------------------------------------------------------------------------------
vec3 GetORM(const vec2 uv)
{
    vec3 ORM = vec3(1.0, SurfaceSpecularColour.r, SurfaceShininessColour.r);
#ifdef HAS_ORM
    ORM *= texture2D(OrmMap, uv, TexLod).rgb;
#endif
    return clamp(ORM, vec3(0.0, F0, 0.0), vec3(1.0));
}


//----------------------------------------------------------------------------------------------------------------------
vec3 GetEmission(const vec2 uv)
{
#ifdef HAS_EMISSIVEMAP
    return SRGBtoLINEAR(SurfaceEmissiveColour.rgb + texture2D(EmissiveMap, uv, TexLod).rgb);
#else
    return SRGBtoLINEAR(SurfaceEmissiveColour.rgb);
#endif
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    highp vec3 v = normalize(CameraPosition - vPosition);
    highp vec2 uv = vUV0.xy;
    uv *= (1.0 + TexScale);

#ifdef HAS_NORMALMAP
#ifdef HAS_PARALLAXMAP
    uv -= (v.xy * (OffsetScale * texture2D(NormalMap, uv, TexLod).a));
#endif // HAS_PARALLAXMAP
#endif // HAS_NORMALMAP

    vec4 s = GetAlbedo(uv);
    vec3 albedo = s.rgb;
    float alpha = s.a;

#ifdef HAS_ALPHA
    if (SurfaceAlphaRejection > 0.0 && alpha < SurfaceAlphaRejection) {
        discard;
    }
#endif

    vec3 ORM = GetORM(uv);
    float occlusion = ORM.r;
    float roughness = ORM.g;
    float metallic = ORM.b;

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    vec3 diffuseColor = albedo * ((1.0 - F0) * (1.0 - metallic));
    vec3 specularColor = mix(vec3(F0), albedo, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    vec3 reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    vec3 reflectance0 = specularColor.rgb;

    // Normal at surface point
    highp vec3 n = GetNormal(uv);
    float NdotV = clamp(dot(n, v), 0.001, 1.0);
    vec3 color = vec3(0.0);

#if MAX_LIGHTS > 0
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        highp vec3 l = -normalize(LightDirectionArray[i].xyz); // Vector from surface point to light
        highp vec3 h = normalize(l + v); // Half vector between both l and v
        highp float NdotL = clamp(dot(n, l), 0.001, 1.0);

        // attenuation is property of spot and point light
        float attenuation = 1.0;
        vec4 vAttParams = LightAttenuationArray[i];
        highp float range = vAttParams.x;

        if (range < HALF_MAX_MINUS1) {
            highp vec3 vLightViewH = LightPositionArray[i].xyz - vPosition;
            highp float fLightD = length(vLightViewH);
            highp float fLightD2 = fLightD * fLightD;
            highp vec3 vLightView = normalize(vLightViewH);
            float attenuation_const = vAttParams.y;
            float attenuation_linear = vAttParams.z;
            float attenuation_quad = vAttParams.w;

            attenuation = biggerhp(range, fLightD) / (attenuation_const + (attenuation_linear * fLightD) + (attenuation_quad * fLightD2));

            // spotlight
            vec3 vSpotParams = LightSpotParamsArray[i].xyz;
            float outer_radius = vSpotParams.z;

            if (outer_radius > 0.0) {
                float fallof = vSpotParams.x;
                float inner_radius = vSpotParams.y;

                float rho = dot(l, vLightView);
                float fSpotE = clamp((rho - inner_radius) / (fallof - inner_radius), 0.0, 1.0);
                attenuation *= pow(fSpotE, outer_radius);
            }
        }

        float light = NdotL * attenuation;
        highp float alphaRoughness = roughness * roughness;
        highp float NdotH = clamp(dot(n, h), 0.0, 1.0);
        float LdotH = clamp(dot(l, h), 0.0, 1.0);
        float VdotH = clamp(dot(v, h), 0.0, 1.0);

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = SpecularReflection(reflectance0, reflectance90, VdotH);
        vec3 diffuseContrib = (1.0 - F) * Diffuse(diffuseColor);

        // Calculation of analytical lighting contribution
        float G = GeometricOcclusion(NdotL, NdotV, alphaRoughness);
        float D = MicrofacetDistribution(alphaRoughness, NdotH);
        vec3 specContrib = (F * (G * D)) / (4.0 * (NdotL * NdotV));

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
            if (LightCastsShadowsArray[i] > 0.0) {
#if MAX_SHADOW_TEXTURES > 2
                light *= GetPSSMShadow(i);
#else
                light *= GetShadow(i);
#endif //  MAX_SHADOW_TEXTURES > 2
            }
#endif //  MAX_SHADOW_TEXTURES > 0
#endif //  SHADOWRECEIVER

        color += light * (LightDiffuseScaledColourArray[i].xyz * (diffuseContrib + specContrib));
    } //  lightning loop
#endif //  MAX_LIGHTS > 0

    vec3 ambient = vec3(0.0);
// Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    vec3 reflection = -normalize(reflect(v, n));
    ambient += occlusion * (SurfaceAmbientColour.rgb * (AmbientLightColour.rgb + GetIBLContribution(BrdfLUT, DiffuseEnvMap, SpecularEnvMap, diffuseColor, specularColor, roughness, NdotV, n, reflection) * albedo));
#else
    ambient += occlusion * (SurfaceAmbientColour.rgb * (AmbientLightColour.rgb * albedo));
#endif // USE_IBL

// Apply optional PBR terms for additional (optional) shading
    color *= occlusion;

    ambient += GetEmission(uv);

    FragData[0] = vec4(color, alpha);
    FragData[1].x = (vDepth - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[2].xyz = n;

    vec2 a = (vScreenPosition.xz / vScreenPosition.w);
    vec2 b = (vPrevScreenPosition.xz / vPrevScreenPosition.w);
    vec2 velocity = ((0.5 * 0.0166667) / FrameTime) * (b - a);
    FragData[3].xy = velocity;
    FragData[4].rg = vec2(metallic, 0.0);
    FragData[5].rgb = ambient;
}

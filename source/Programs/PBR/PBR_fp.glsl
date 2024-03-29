// This source file is part of Glue Engine. Created by Andrey Vasiliev

// The MIT License
// Copyright (c) 2016-2017 Mohamad Moneimne and Contributors
//
// This fragment shader defines a reference implementation for Physically Based Shading of
// a microfacet surface material defined by a glTF model.
//
// References:
// [1] Real Shading in Unreal Engine 4
//     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [2] Physically Based Shading at Disney
//     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [3] README.md - Environment Maps
//     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
// [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
//     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf

#ifndef VERSION
#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 300 es
#define VERSION 300
#endif
#endif

#ifdef SHADOWCASTER
#define NO_MRT
#endif

#include "header.frag"

#ifdef SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
uniform sampler2D uAlbedoSampler;
uniform float SurfaceAlphaRejection;
in vec2 vUV0;
#endif
#endif

#ifndef SHADOWCASTER
#ifdef HAS_BASECOLORMAP
uniform sampler2D uAlbedoSampler;
#endif
#ifdef HAS_NORMALMAP
uniform sampler2D uNormalSampler;
#endif
#ifdef HAS_ORM
uniform sampler2D uORMSampler;
#else
#ifdef HAS_OCCLUSIONMAP
uniform sampler2D uOcclusionSampler;
#endif
#ifdef HAS_ROUGHNESSMAP
uniform sampler2D uRoughnessSampler;
#endif
#ifdef HAS_METALLICMAP
uniform sampler2D uMetallicSampler;
#endif
#endif
#ifdef HAS_EMISSIVEMAP
uniform sampler2D uEmissiveSampler;
#endif
#ifdef USE_IBL
uniform samplerCube uDiffuseEnvSampler;
uniform samplerCube uSpecularEnvSampler;
uniform sampler2D uBrdfLUT;
#endif
#ifdef TERRAIN
uniform sampler2D uGlobalNormalSampler;
#endif
#ifdef HAS_REFLECTION
uniform sampler2D uReflectionMap;
#endif
#ifdef SHADOWRECEIVER
#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 0
#endif
#if MAX_SHADOW_TEXTURES > 0
uniform sampler2D uShadowMap0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform sampler2D uShadowMap1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform sampler2D uShadowMap2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform sampler2D uShadowMap3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform sampler2D uShadowMap4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform sampler2D uShadowMap5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform sampler2D uShadowMap6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform sampler2D uShadowMap7;
#endif
#endif // SHADOWRECEIVER

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 0
#endif
#if MAX_LIGHTS > 0
uniform vec4 LightPositionArray[MAX_LIGHTS];
uniform vec4 LightDirectionArray[MAX_LIGHTS];
uniform vec4 LightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 LightAttenuationArray[MAX_LIGHTS];
uniform vec4 LightSpotParamsArray[MAX_LIGHTS];
#ifdef SHADOWRECEIVER
uniform float LightCastsShadowsArray[MAX_LIGHTS];
#endif
#endif
uniform float LightCount;
#ifndef USE_IBL
uniform vec3 AmbientLightColour;
#endif
uniform vec3 SurfaceAmbientColour;
uniform vec3 SurfaceDiffuseColour;
uniform float SurfaceSpecularColour;
uniform float SurfaceShininessColour;
uniform vec3 SurfaceEmissiveColour;
#ifdef HAS_ALPHA
uniform float SurfaceAlphaRejection;
#endif
#ifdef NO_MRT
uniform vec3 FogColour;
uniform vec4 FogParams;
#endif
#ifndef NO_MRT
uniform float FarClipDistance;
uniform float FrameTime;
#endif
uniform float uLOD;
uniform vec3 CameraPosition;
#ifdef TERRAIN
uniform float uTerrainTexScale;
#endif
#ifdef HAS_NORMALMAP
uniform float uNormalScale;
#endif
#ifdef HAS_PARALLAXMAP
uniform float uOffsetScale;
#endif
#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
uniform float ShadowTexel0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform float ShadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform float ShadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform float ShadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform float ShadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform float ShadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform float ShadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform float ShadowTexel7;
#endif
uniform vec4 uPssmSplitPoints;
uniform vec3 ShadowColour;
uniform float ShadowDepthOffset;
uniform float ShadowFilterSize;
uniform int ShadowFilterIterations;
#endif // SHADOWRECEIVER

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
in vec4 LightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
#endif
in vec2 vUV0;
in float vDepth;
#ifdef FADE
in float vAlpha;
#endif
in vec3 vPosition;
#ifndef NO_MRT
in vec4 vScreenPosition;
in vec4 vPrevScreenPosition;
#endif
#ifdef HAS_COLOURS
in vec3 vColor;
#endif
#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
in mat3 vTBN;
#else
in vec3 vNormal;
#endif
#endif
#ifdef HAS_REFLECTION
in vec4 projectionCoord;
#endif

//SRGB corretion
#include "srgb.glsl"
#ifdef NO_MRT
#include "fog.glsl"
#endif
#include "lighting.glsl"
#ifdef USE_IBL
#include "ibl.glsl"
#endif
#ifdef SHADOWRECEIVER
#include "receiver.glsl"
#endif
#ifdef HAS_REFLECTION
#include "noise.glsl"
#endif

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
//----------------------------------------------------------------------------------------------------------------------
float GetShadow(const int counter) {
    if (vDepth >= uPssmSplitPoints.w)
        return 1.0;
#if MAX_SHADOW_TEXTURES > 0
    else if (counter == 0)
        return CalcDepthShadow(uShadowMap0, LightSpacePosArray[0], ShadowDepthOffset, ShadowTexel0*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (counter == 1)
        return CalcDepthShadow(uShadowMap1, LightSpacePosArray[1], ShadowDepthOffset, ShadowTexel1*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (counter == 2)
        return CalcDepthShadow(uShadowMap2, LightSpacePosArray[2], ShadowDepthOffset, ShadowTexel2*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (counter == 3)
        return CalcDepthShadow(uShadowMap3, LightSpacePosArray[3], ShadowDepthOffset, ShadowTexel3*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (counter == 4)
        return CalcDepthShadow(uShadowMap4, LightSpacePosArray[4], ShadowDepthOffset, ShadowTexel4*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (counter == 5)
        return CalcDepthShadow(uShadowMap5, LightSpacePosArray[5], ShadowDepthOffset, ShadowTexel5*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (counter == 6)
        return CalcDepthShadow(uShadowMap6, LightSpacePosArray[6], ShadowDepthOffset, ShadowTexel6*ShadowFilterSize, ShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (counter == 7)
        return CalcDepthShadow(uShadowMap7, LightSpacePosArray[7], ShadowDepthOffset, ShadowTexel7*ShadowFilterSize, ShadowFilterIterations);
#endif

    return 1.0;
}

//----------------------------------------------------------------------------------------------------------------------
float CalcPSSMDepthShadow(const vec4 uPssmSplitPoints, const vec4 lightSpacePos0, const vec4 lightSpacePos1, const vec4 lightSpacePos2, const sampler2D uShadowMap0, const sampler2D uShadowMap1, const sampler2D uShadowMap2)
{
    // calculate shadow
    if (vDepth <= uPssmSplitPoints.x)
        return CalcDepthShadow(uShadowMap0, lightSpacePos0, ShadowDepthOffset, ShadowTexel0*ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= uPssmSplitPoints.y)
        return CalcDepthShadow(uShadowMap1, lightSpacePos1, ShadowDepthOffset, ShadowTexel1*ShadowFilterSize, ShadowFilterIterations);
    else if (vDepth <= uPssmSplitPoints.z)
        return CalcDepthShadow(uShadowMap2, lightSpacePos2, ShadowDepthOffset, ShadowTexel2*ShadowFilterSize, ShadowFilterIterations);
    else
        return 1.0;
}
#endif // MAX_SHADOW_TEXTURES > 0
#endif // SHADOWRECEIVER

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
//----------------------------------------------------------------------------------------------------------------------
mat3 GetTGN()
{
// Retrieve the tangent space matrix
#ifndef HAS_TANGENTS
    vec3 t = vec3(1, 0, 0);
    vec3 pos_dx = dFdx(vPosition);
    vec3 pos_dy = dFdy(vPosition);

#ifdef HAS_NORMALS
    vec3 ng = normalize(vNormal);
#else //!HAS_NORMALS
    vec3 ng = cross(pos_dx, pos_dy);
#endif //HAS_NORMALS

    vec3 b = normalize(cross(ng, t));
    t = normalize(cross(ng ,b));
    return mat3(t, b, ng);

#else //HAS_TANGENTS
    return vTBN;
#endif //!HAS_TANGENTS
}

#ifdef TERRAIN
//----------------------------------------------------------------------------------------------------------------------
mat3 GetTgnTerrain()
{
    // Retrieve the tangent space matrix
#ifndef HAS_TANGENTS
    vec3 t = vec3(1.0, 0.0, 0.0);
    vec3 ng = texture2D(uGlobalNormalSampler, vUV0.xy).xyz * 2.0 - 1.0;
    vec3 b = normalize(cross(ng, t));
    t = normalize(cross(ng ,b));
    mat3 tbn = mat3(t, b, ng);
#else //HAS_TANGENTS
    mat3 tbn = vTBN;
#endif //!HAS_TANGENTS

    return tbn;
}
#endif // TERRAIN

//----------------------------------------------------------------------------------------------------------------------
vec3 GetNormal(const vec2 uv)
{
#ifdef TERRAIN
    mat3 tbn = GetTgnTerrain();
#else
    mat3 tbn = GetTGN();
#endif

#ifdef HAS_NORMALMAP
    vec3 n = uNormalScale * texture2D(uNormalSampler, uv, uLOD).xyz;
    n = normalize(tbn * ((2.0 * n - 1.0)));
#else //!HAS_NORMALMAP
    vec3 n = tbn[2].xyz;
#endif

    return n;
}

//----------------------------------------------------------------------------------------------------------------------
vec4 GetAlbedo(const vec2 uv) {
#ifdef HAS_COLOURS
    vec4 albedo = vec4(SurfaceDiffuseColour * vColor, 1.0);
#else
    vec4 albedo = vec4(SurfaceDiffuseColour, 1.0);
#endif
#ifdef HAS_BASECOLORMAP
    albedo *= texture2D(uAlbedoSampler, uv, uLOD);
#endif
    return SRGBtoLINEAR(albedo);
}

#if 0
//----------------------------------------------------------------------------------------------------------------------
float GetMetallic(const vec2 uv) {
    float metallic = SurfaceShininessColour;
#ifdef HAS_METALLICMAP
    metallic *= texture2D(uMetallicSampler, uv, uLOD).r;
#endif
    return metallic;
}

//----------------------------------------------------------------------------------------------------------------------
float GetRoughness(const vec2 uv) {
    float roughness = SurfaceSpecularColour;
#ifdef HAS_ROUGHNESSMAP
    roughness *= texture2D(uRoughnessSampler, uv, uLOD).r;
#endif
    return roughness;
}

//----------------------------------------------------------------------------------------------------------------------
float GetOcclusion(const vec2 uv) {
#ifdef HAS_OCCLUSIONMAP
    return texture2D(uOcclusionSampler, uv, uLOD).r;
#else
    return 1.0;
#endif
}
#endif // 0

//----------------------------------------------------------------------------------------------------------------------
vec3 GetORM(const vec2 uv) {
    vec3 ORM = vec3(1.0, SurfaceSpecularColour, SurfaceShininessColour);
#ifdef HAS_ORM
    ORM *= texture2D(uORMSampler, uv, uLOD).rgb;
#endif
    return ORM;
}

#ifdef HAS_REFLECTION
//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyReflection(const vec3 color, const vec3 n, const vec3 v, const float metallic) {
    vec4 projection = projectionCoord;

    const float fresnelBias = 0.1;
    const float fresnelScale = 1.8;
    const float fresnelPower = 8.0;
    const float filter_max_size = 0.1;
    const int sample_count = 8;

    float cosa = dot(n, -v);
    float fresnel = fresnelBias + fresnelScale * pow(1.0 + cosa, fresnelPower);
    fresnel = clamp(fresnel, 0.0, 1.0);
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    projection.xy += VogelDiskSample(3, sample_count, gradientNoise) * filter_max_size;
    vec3 reflectionColour = texture2DProj(uReflectionMap, projection).rgb;
    return mix(color, reflectionColour, fresnel * metallic);
}
#endif

#ifdef HAS_PARALLAXMAP
//----------------------------------------------------------------------------------------------------------------------
vec2 ParallaxMapping(const vec2 uv, const vec3 viewDir)
{
    float displacement = uOffsetScale * texture2D(uNormalSampler, uv, uLOD).a;
    return uv - viewDir.xy * displacement;
}
#endif

#endif // !SHADOWCASTER

//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifndef SHADOWCASTER
    vec3 v = normalize(CameraPosition - vPosition);
    vec2 tex_coord = vUV0.xy;

#ifdef TERRAIN
    tex_coord *= uTerrainTexScale;
#endif

#ifdef HAS_PARALLAXMAP
    tex_coord = ParallaxMapping(tex_coord, v);
#endif

    vec4 albedo = GetAlbedo(tex_coord);
    vec3 color = albedo.rgb;
    float alpha = albedo.a;

#ifdef HAS_ALPHA
#ifdef FADE
    alpha *= vAlpha;
#endif
    if (alpha < SurfaceAlphaRejection) discard;
#endif // HAS_ALPHA

    vec3 ORM = GetORM(tex_coord);
    float occlusion = ORM.r;
    float roughness = ORM.g;
    float metallic = ORM.b;

    roughness = clamp(roughness, F0, 1.0);

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    vec3 diffuseColor = color * vec3(1.0 - F0);
    diffuseColor *= (1.0 - metallic);
    vec3 specularColor = mix(vec3(F0), color, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    vec3 reflectance90 = vec3(clamp(reflectance * 25.0, 0.0, 1.0));
    vec3 reflectance0 = specularColor.rgb;

    vec3 n = GetNormal(tex_coord);// normal at surface point

    float NdotV = abs(dot(n, v)) + 0.001;

    vec3 total_colour = vec3(0.0);

#if MAX_LIGHTS > 0
    for (int i = 0; i < int(LightCount); i++) {
        vec3 l = -normalize(LightDirectionArray[i].xyz); // Vector from surface point to light
        vec3 h = normalize(l+v); // Half vector between both l and v
        float NdotL = clamp(dot(n, l), 0.001, 1.0);

        vec3 vLightView = LightPositionArray[i].xyz - vPosition;
        float fLightD = length(vLightView);
        vLightView = normalize(vLightView);

        float fAtten = 1.0;
        float fSpotT = 1.0;

        vec4 vAttParams = LightAttenuationArray[i];
        float range = vAttParams.x;

        if (range > 0.0) {
            float attenuation_const = vAttParams.y;
            float attenuation_linear = vAttParams.z;
            float attenuation_quad = vAttParams.w;

            fAtten = float(fLightD < range) / (attenuation_const + (attenuation_linear * fLightD) + (attenuation_quad * (fLightD * fLightD)));

            vec3 vSpotParams = LightSpotParamsArray[i].xyz;
            float outer_radius = vSpotParams.z;

            if (outer_radius > 0.0) {
                float fallof = vSpotParams.x;
                float inner_radius = vSpotParams.y;

                float rho = dot(l, vLightView);
                float fSpotE = clamp((rho - inner_radius) / (fallof - inner_radius), 0.0, 1.0);
                fSpotT = pow(fSpotE, outer_radius);
              }
        }

        float NdotH = clamp(dot(n, h), 0.0, 1.0);
        float LdotH = clamp(dot(l, h), 0.0, 1.0);
        float VdotH = clamp(dot(v, h), 0.0, 1.0);
        float alphaRoughness = (roughness * roughness);

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = SpecularReflection(reflectance0, reflectance90, VdotH);
        vec3 diffuseContrib = (1.0 - F) * Diffuse(diffuseColor);

        // Calculation of analytical lighting contribution
        float G = GeometricOcclusion(NdotL, NdotV, alphaRoughness);
        float D = MicrofacetDistribution(alphaRoughness, NdotH);
        vec3 specContrib = (F * (G * D)) / (4.0 * (NdotL * NdotV));
        float tmp = (NdotL * (fSpotT * fAtten));

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
        float shadow = 1.0;

        if (LightCastsShadowsArray[i] > 0.0) {
            #define EPSILON 0.000001

            #if MAX_SHADOW_TEXTURES > 2
                if (LightAttenuationArray[0].x < 100000.0) {
                    shadow = (tmp > EPSILON) ? GetShadow(i) : 1.0;
                } else {
                    if (i == 0) {
                        shadow = (tmp > EPSILON) ? CalcPSSMDepthShadow(uPssmSplitPoints, \
                                                    LightSpacePosArray[0], LightSpacePosArray[1], LightSpacePosArray[2], \
                                                    uShadowMap0, uShadowMap1, uShadowMap2) : 1.0;
                    } else {
                        shadow = (tmp > EPSILON) ? GetShadow(i + 2) : 1.0;
                    }
                }
            #else
                shadow = (tmp > EPSILON) ? GetShadow(i) : 1.0;
            #endif
        }

        total_colour += ShadowColour * color + (shadow * (tmp * (LightDiffuseScaledColourArray[i].xyz * (diffuseContrib + specContrib))));
#endif
#else
    total_colour += (tmp * (LightDiffuseScaledColourArray[i].xyz * (diffuseContrib + specContrib)));
#endif
    }
#endif

// Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    vec3 reflection = -normalize(reflect(v, n));
    total_colour += SurfaceAmbientColour * GetIBLContribution(uBrdfLUT, uDiffuseEnvSampler, uSpecularEnvSampler, diffuseColor, specularColor, roughness, NdotV, n, reflection);
#else
    total_colour += (SurfaceAmbientColour * (AmbientLightColour * color));
#endif

#ifdef HAS_REFLECTION
    total_colour = ApplyReflection(total_colour, n, v, metallic);
#endif //HAS_REFLECTION

// Apply optional PBR terms for additional (optional) shading
#ifdef HAS_OCCLUSIONMAP
    total_colour *= occlusion;
#endif

#ifdef HAS_EMISSIVEMAP
    total_colour += SRGBtoLINEAR(SurfaceEmissiveColour + texture2D(uEmissiveSampler, tex_coord, uLOD).rgb);
#else
    total_colour += SRGBtoLINEAR(SurfaceEmissiveColour);
#endif

#ifndef NO_MRT
    FragData[0] = vec4(total_colour, alpha);

    FragData[1].r = (vDepth / FarClipDistance);

    vec2 a = (vScreenPosition.xz / vScreenPosition.w);
    vec2 b = (vPrevScreenPosition.xz / vPrevScreenPosition.w);
    vec2 velocity = (0.0166667 / FrameTime) * 0.5 * vec2(a - b);
    FragData[2].rg = velocity;
#else
    total_colour = ApplyFog(total_colour, FogParams, FogColour, vDepth);
    FragColor = LINEARtoSRGB(vec4(total_colour, alpha), 1.0);
#endif

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(uAlbedoSampler, vUV0.xy).a < SurfaceAlphaRejection) discard;
#endif //SHADOWCASTER_ALPHA
#ifdef NO_MRT
    FragColor.r = gl_FragCoord.z;
#else
    FragData[0].r = gl_FragCoord.z;
#endif
#endif //SHADOWCASTER
}

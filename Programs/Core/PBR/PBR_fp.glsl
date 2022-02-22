// This source file is part of "glue project". Created by Andrey Vasiliev

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
#version 100
#define VERSION 100
#endif
#endif

#ifdef SHADOWCASTER
#define NO_MRT
#endif

#include "header.frag"

#ifdef SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
uniform sampler2D uAlbedoSampler;
uniform float uAlphaRejection;
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
uniform sampler2D ubrdfLUT;
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
uniform sampler2D shadowMap0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform sampler2D shadowMap1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform sampler2D shadowMap2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform sampler2D shadowMap3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform sampler2D shadowMap4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform sampler2D shadowMap5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform sampler2D shadowMap6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform sampler2D shadowMap7;
#endif
#endif // SHADOWRECEIVER

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 0
#endif
#if MAX_LIGHTS > 0
uniform vec4 uLightPositionArray[MAX_LIGHTS];
uniform vec4 uLightDirectionArray[MAX_LIGHTS];
uniform vec4 uLightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 uLightAttenuationArray[MAX_LIGHTS];
uniform vec4 uLightSpotParamsArray[MAX_LIGHTS];
#ifdef SHADOWRECEIVER
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
#endif
#endif
uniform float uLightCount;
#ifndef USE_IBL
uniform vec3 uAmbientLightColour;
#endif
uniform vec3 uSurfaceAmbientColour;
uniform vec3 uSurfaceDiffuseColour;
uniform float uSurfaceSpecularColour;
uniform float uSurfaceShininessColour;
uniform vec3 uSurfaceEmissiveColour;
#ifdef NO_MRT
uniform vec3 uFogColour;
uniform vec4 uFogParams;
#endif
#ifndef NO_MRT
uniform float uFarClipDistance;
uniform float uFrameTime;
#endif
uniform float uLOD;
uniform vec3 uCameraPosition;
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
uniform float shadowTexel0;
#endif
#if MAX_SHADOW_TEXTURES > 1
uniform float shadowTexel1;
#endif
#if MAX_SHADOW_TEXTURES > 2
uniform float shadowTexel2;
#endif
#if MAX_SHADOW_TEXTURES > 3
uniform float shadowTexel3;
#endif
#if MAX_SHADOW_TEXTURES > 4
uniform float shadowTexel4;
#endif
#if MAX_SHADOW_TEXTURES > 5
uniform float shadowTexel5;
#endif
#if MAX_SHADOW_TEXTURES > 6
uniform float shadowTexel6;
#endif
#if MAX_SHADOW_TEXTURES > 7
uniform float shadowTexel7;
#endif
uniform vec4 pssmSplitPoints;
uniform vec3 uShadowColour;
uniform float uShadowDepthOffset;
uniform float uShadowFilterSize;
uniform int uShadowFilterIterations;
#endif // SHADOWRECEIVER

#ifdef SHADOWRECEIVER
#if MAX_SHADOW_TEXTURES > 0
in vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES];
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
//----------------------------------------------------------------------------------------------------------------------
float GetShadow(const int counter) {
    if (vDepth >= pssmSplitPoints.w)
        return 1.0;
#if MAX_SHADOW_TEXTURES > 0
    else if (counter == 0)
        return CalcDepthShadow(shadowMap0, lightSpacePosArray[0], uShadowDepthOffset, shadowTexel0*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 1
    else if (counter == 1)
        return CalcDepthShadow(shadowMap1, lightSpacePosArray[1], uShadowDepthOffset, shadowTexel1*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (counter == 2)
        return CalcDepthShadow(shadowMap2, lightSpacePosArray[2], uShadowDepthOffset, shadowTexel2*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (counter == 3)
        return CalcDepthShadow(shadowMap3, lightSpacePosArray[3], uShadowDepthOffset, shadowTexel3*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (counter == 4)
        return CalcDepthShadow(shadowMap4, lightSpacePosArray[4], uShadowDepthOffset, shadowTexel4*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (counter == 5)
        return CalcDepthShadow(shadowMap5, lightSpacePosArray[5], uShadowDepthOffset, shadowTexel5*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (counter == 6)
        return CalcDepthShadow(shadowMap6, lightSpacePosArray[6], uShadowDepthOffset, shadowTexel6*uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (counter == 7)
        return CalcDepthShadow(shadowMap7, lightSpacePosArray[7], uShadowDepthOffset, shadowTexel7*uShadowFilterSize, uShadowFilterIterations);
#endif

    return 1.0;
}

//----------------------------------------------------------------------------------------------------------------------
float CalcPSSMDepthShadow(const vec4 pssmSplitPoints, const vec4 lightSpacePos0, const vec4 lightSpacePos1, const vec4 lightSpacePos2, const sampler2D shadowMap0, const sampler2D shadowMap1, const sampler2D shadowMap2)
{
    // calculate shadow
    if (vDepth <= pssmSplitPoints.x)
        return CalcDepthShadow(shadowMap0, lightSpacePos0, uShadowDepthOffset, shadowTexel0*uShadowFilterSize, uShadowFilterIterations);
    else if (vDepth <= pssmSplitPoints.y)
        return CalcDepthShadow(shadowMap1, lightSpacePos1, uShadowDepthOffset, shadowTexel1*uShadowFilterSize, uShadowFilterIterations);
    else if (vDepth <= pssmSplitPoints.z)
        return CalcDepthShadow(shadowMap2, lightSpacePos2, uShadowDepthOffset, shadowTexel2*uShadowFilterSize, uShadowFilterIterations);

    return 1.0;
}
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
    vec4 albedo = vec4(uSurfaceDiffuseColour * vColor, 1.0);
#else
    vec4 albedo = vec4(uSurfaceDiffuseColour, 1.0);
#endif
#ifdef HAS_BASECOLORMAP
    albedo *= texture2D(uAlbedoSampler, uv, uLOD);
#endif
    return SRGBtoLINEAR(albedo);
}

#if 0
//----------------------------------------------------------------------------------------------------------------------
float GetMetallic(const vec2 uv) {
    float metallic = uSurfaceShininessColour;
#ifdef HAS_METALLICMAP
    metallic *= texture2D(uMetallicSampler, uv, uLOD).r;
#endif
    return clamp(metallic, 0.0, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------
float GetRoughness(const vec2 uv) {
    float roughness = uSurfaceSpecularColour;
#ifdef HAS_ROUGHNESSMAP
    roughness *= texture2D(uRoughnessSampler, uv, uLOD).r;
#endif
    return clamp(roughness, F0, 1.0);
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
    vec3 ORM = vec3(1.0, uSurfaceSpecularColour, uSurfaceShininessColour);
#ifdef HAS_ORM
    vec3 texel = texture2D(uORMSampler, uv, uLOD).rgb;
    ORM.r = texel.r;
    ORM.g *= texel.g;
    ORM.b *= texel.b;
#endif
    //ORM.g = clamp(ORM.g, 0.0, 1.0);
    ORM.b = clamp(ORM.b, F0, 1.0);
    return ORM;
}

#ifdef HAS_REFLECTION
//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyReflection(vec3 color, vec3 n, vec3 v, float metallic) {
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
vec2 ParallaxMapping(vec2 uv, vec3 viewDir)
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
    vec3 v = normalize(uCameraPosition - vPosition);
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
    if (alpha < 0.5) discard;
#endif // HAS_ALPHA

    vec3 ORM = GetORM(tex_coord);
    float occlusion = ORM.r;
    float roughness = ORM.g;
    float metallic = ORM.b;

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
    for (int i = 0; i < int(uLightCount); i++) {
        vec3 l = -normalize(uLightDirectionArray[i].xyz); // Vector from surface point to light
        vec3 h = normalize(l+v); // Half vector between both l and v
        float NdotL = clamp(dot(n, l), 0.001, 1.0);

        vec3 vLightView = uLightPositionArray[i].xyz - vPosition;
        float fLightD = length(vLightView);
        vLightView = normalize(vLightView);

        float fAtten = 1.0;
        float fSpotT = 1.0;

        vec4 vAttParams = uLightAttenuationArray[i];
        float range = vAttParams.x;

        if (range > 0.0) {
            float attenuation_const = vAttParams.y;
            float attenuation_linear = vAttParams.z;
            float attenuation_quad = vAttParams.w;

            fAtten = float(fLightD < range) / (attenuation_const + (attenuation_linear * fLightD) + (attenuation_quad * (fLightD * fLightD)));

            vec3 vSpotParams = uLightSpotParamsArray[i].xyz;
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

        if (uLightCastsShadowsArray[i] > 0.0) {
            #define EPSILON 0.000001

            #if MAX_SHADOW_TEXTURES > 2
                if (uLightAttenuationArray[0].x < 100000.0) {
                    shadow = (tmp > EPSILON) ? GetShadow(i) : 1.0;
                } else {
                    if (i == 0) {
                        shadow = (tmp > EPSILON) ? CalcPSSMDepthShadow(pssmSplitPoints, \
                                                    lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], \
                                                    shadowMap0, shadowMap1, shadowMap2) : 1.0;
                    } else {
                        shadow = (tmp > EPSILON) ? GetShadow(i + 2) : 1.0;
                    }
                }
            #else
                shadow = (tmp > EPSILON) ? GetShadow(i) : 1.0;
            #endif
        }

        total_colour += uShadowColour * color + (shadow * (tmp * (uLightDiffuseScaledColourArray[i].xyz * (diffuseContrib + specContrib))));
#endif
#else
    total_colour += (tmp * (uLightDiffuseScaledColourArray[i].xyz * (diffuseContrib + specContrib)));
#endif
    }
#endif

// Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    vec3 reflection = -normalize(reflect(v, n));
    total_colour += uSurfaceAmbientColour * GetIBLContribution(ubrdfLUT, uDiffuseEnvSampler, uSpecularEnvSampler, diffuseColor, specularColor, roughness, NdotV, n, reflection);
#else
    total_colour += (uSurfaceAmbientColour * (uAmbientLightColour * color));
#endif

#ifdef HAS_REFLECTION
    total_colour = ApplyReflection(total_colour, n, v, metallic);
#endif //HAS_REFLECTION

// Apply optional PBR terms for additional (optional) shading
#ifdef HAS_OCCLUSIONMAP
    total_colour *= occlusion;
#endif

#ifdef HAS_EMISSIVEMAP
    total_colour += SRGBtoLINEAR(uSurfaceEmissiveColour + texture2D(uEmissiveSampler, tex_coord, uLOD).rgb);
#else
    total_colour += SRGBtoLINEAR(uSurfaceEmissiveColour);
#endif

#ifndef NO_MRT
    FragData[0] = vec4(total_colour, alpha);

    FragData[1].r = (vDepth / uFarClipDistance);

    vec2 a = (vScreenPosition.xz / vScreenPosition.w);
    vec2 b = (vPrevScreenPosition.xz / vPrevScreenPosition.w);
    vec2 velocity = (0.0166667 / uFrameTime) * 0.5 * vec2(a - b);
    FragData[2].rg = velocity;
#else
    total_colour = ApplyFog(total_colour, uFogParams, uFogColour, vDepth);
    FragColor = LINEARtoSRGB(vec4(total_colour, alpha), 1.0);
#endif

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(uAlbedoSampler, vUV0.xy).a < 0.5) discard;
#endif //SHADOWCASTER_ALPHA
#ifdef NO_MRT
    FragColor.r = gl_FragCoord.z;
#else
    FragData[0].r = gl_FragCoord.z;
#endif
#endif //SHADOWCASTER
}

//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

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

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.frag"

#ifdef SHADOWCASTER_ALPHA
in vec2 vUV0;
uniform sampler2D uAlbedoSampler;
#endif

#ifndef SHADOWCASTER
in vec2 vUV0;
in float vDepth;
in float vAlpha;
#ifdef HAS_BASECOLORMAP
uniform sampler2D uAlbedoSampler;
#endif
uniform float uLOD;
uniform float uAlphaRejection;
uniform vec3 uSurfaceAmbientColour;
uniform vec3 uSurfaceDiffuseColour;
uniform float uSurfaceSpecularColour;
uniform float uSurfaceShininessColour;
uniform vec3 uSurfaceEmissiveColour;
uniform vec3 uAmbientLightColour;
uniform float uLightCount;
uniform vec3 uLightPositionArray[MAX_LIGHTS];
uniform vec3 uLightDirectionArray[MAX_LIGHTS];
uniform vec3 uLightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec4 uLightAttenuationArray[MAX_LIGHTS];
uniform vec3 uLightSpotParamsArray[MAX_LIGHTS];
#ifdef SHADOWRECEIVER
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
#endif
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform vec3 uCameraPosition;
#ifdef USE_IBL
uniform samplerCube uDiffuseEnvSampler;
uniform samplerCube uSpecularEnvSampler;
uniform sampler2D ubrdfLUT;
#endif
#ifdef TERRAIN
uniform sampler2D uGlobalNormalSampler;
//uniform sampler2D uGlobalShadowSampler;
uniform float uTerrainTexScale;
#endif
#ifdef HAS_NORMALMAP
uniform sampler2D uNormalSampler;
uniform float uNormalScale;
#endif
#ifdef HAS_EMISSIVEMAP
uniform sampler2D uEmissiveSampler;
#endif
#ifdef HAS_METALLICMAP
uniform sampler2D uMetallicSampler;
#endif
#ifdef HAS_ROUGHNESSMAP
uniform sampler2D uRoughnessSampler;
#endif
#ifdef HAS_OCCLUSIONMAP
uniform sampler2D uOcclusionSampler;
#endif
#ifdef HAS_PARALLAXMAP
uniform sampler2D uOffsetSampler;
uniform float uOffsetScale;
#endif


#ifdef SHADOWRECEIVER

uniform sampler2D shadowMap0;
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

uniform vec4 pssmSplitPoints;
uniform float uShadowColour;
uniform float uShadowDepthOffset;
uniform float uShadowFilterSize;
uniform int uShadowFilterIterations;

in vec4 lightSpacePosArray[MAX_SHADOW_TEXTURES];
#endif
in vec3 vPosition;
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
uniform sampler2D uReflectionMap;
in vec4 projectionCoord;
#endif

const float M_PI = 3.141592653589793;

//SRGB corretion
#include "srgb.glsl"

//Shadows block
#ifdef SHADOWRECEIVER
#include "receiver.glsl"

float GetShadow(const int counter, float offset, float filter_size, int filter_iterations) {
    if (vDepth>= pssmSplitPoints.w)
      return 1.0;
    else if (counter == 0)
      return CalcDepthShadow(shadowMap0, lightSpacePosArray[0], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#if MAX_SHADOW_TEXTURES > 1
    else if (counter == 1)
      return CalcDepthShadow(shadowMap1, lightSpacePosArray[1], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 2
    else if (counter == 2)
      return CalcDepthShadow(shadowMap2, lightSpacePosArray[2], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 3
    else if (counter == 3)
      return CalcDepthShadow(shadowMap3, lightSpacePosArray[3], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 4
    else if (counter == 4)
      return CalcDepthShadow(shadowMap4, lightSpacePosArray[4], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 5
    else if (counter == 5)
      return CalcDepthShadow(shadowMap5, lightSpacePosArray[5], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 6
    else if (counter == 6)
      return CalcDepthShadow(shadowMap6, lightSpacePosArray[6], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif
#if MAX_SHADOW_TEXTURES > 7
    else if (counter == 7)
      return CalcDepthShadow(shadowMap7, lightSpacePosArray[7], uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations);
#endif

    return 1.0;
}

#endif


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

//----------------------------------------------------------------------------------------------------------------------
#ifdef TERRAIN
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
#endif

//----------------------------------------------------------------------------------------------------------------------
vec3 GetNormal(vec2 uv)
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
float GetMetallic(vec2 uv) {
    float metallic = uSurfaceShininessColour;

#ifdef HAS_METALLICMAP
    metallic *= texture2D(uMetallicSampler, uv, uLOD).r;
#endif

    return metallic;
}

//----------------------------------------------------------------------------------------------------------------------
float GetRoughness(vec2 uv) {
    const float c_MinRoughness = 0.04;
    float roughness = uSurfaceSpecularColour;

#ifdef HAS_ROUGHNESSMAP
    roughness *= texture2D(uRoughnessSampler, uv, uLOD).r;
#endif

    return roughness > c_MinRoughness ? roughness : c_MinRoughness;
}

//----------------------------------------------------------------------------------------------------------------------
vec4 GetAlbedo(vec2 uv) {
    vec4 albedo = vec4(1.0);

#ifdef HAS_ALPHA
    albedo = texture2D(uAlbedoSampler, uv, uLOD);
#else
    albedo = vec4(texture2D(uAlbedoSampler, uv, uLOD).rgb, 1.0);
#endif

#ifdef HAS_COLOURS
    albedo.rgb *= (uSurfaceDiffuseColour * vColor);
#else
    albedo.rgb *= (uSurfaceDiffuseColour);
#endif

    return SRGBtoLINEAR(albedo);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyFog(vec3 color) {
    const float maxy = 500.0;
    float exponent = vDepth * uFogParams.x;
    float fog_value = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);
    fog_value *= ((maxy - vPosition.y) / maxy);
    return mix(color, uFogColour, fog_value);
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_REFLECTION
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
    float displacement = uOffsetScale * texture2D(uOffsetSampler, uv, uLOD).r;
    return uv - viewDir.xy * displacement;
}
#endif

#include "lighting.glsl"

#endif //!SHADOWCASTER

#ifdef USE_IBL
#include "ibl.glsl"
#endif

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

    // The albedo may be defined from a base texture or a flat color
    vec4 albedo = GetAlbedo(tex_coord);
    float metallic = GetMetallic(tex_coord);
    float roughness = GetRoughness(tex_coord);

#ifdef HAS_ALPHA
#ifdef PAGED_GEOMETRY
    albedo.a *= vAlpha;
#endif //PAGED_GEOMETRY

    if (albedo.a < uAlphaRejection)
        discard;
#endif

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    const vec3 f0 = vec3(0.04);
    vec3 diffuseColor = albedo.rgb * (vec3(1.0) - f0);
    diffuseColor *= (1.0 - metallic);
    vec3 specularColor = mix(f0, albedo.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0) * reflectance90;

    vec3 n = GetNormal(tex_coord);// normal at surface point

    float NdotV = abs(dot(n, v)) + 0.001;

    vec3 total_colour = vec3(0.0);

    for (int i = 0; i < int(uLightCount); i++) {
        vec3 l = -normalize(uLightDirectionArray[i]);// Vector from surface point to light
        vec3 h = normalize(l+v);// Half vector between both l and v
        float NdotL = clamp(dot(n, l), 0.001, 1.0);

        vec3 vLightView = uLightPositionArray[i] - vPosition;
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

            fAtten = float(fLightD < range) / (attenuation_const + attenuation_linear * fLightD + attenuation_quad * fLightD * fLightD);

            vec3 vSpotParams = uLightSpotParamsArray[i];
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

        PBRInfo pbrInputs = PBRInfo(
                                    NdotL,
                                    NdotV,
                                    NdotH,
                                    LdotH,
                                    VdotH,
                                    roughness,
                                    metallic,
                                    specularEnvironmentR0,
                                    specularEnvironmentR90,
                                    roughness * roughness,
                                    diffuseColor,
                                    specularColor
                                    );

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = SpecularReflection(pbrInputs);
        vec3 diffuseContrib = (1.0 - F) * Diffuse(pbrInputs);

        // Calculation of analytical lighting contribution
        float G = GeometricOcclusion(pbrInputs);
        float D = MicrofacetDistribution(pbrInputs);
        vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);

        float tmp = NdotL * fSpotT * fAtten;
        vec3 colour = tmp * uLightDiffuseScaledColourArray[i] * (diffuseContrib + specContrib);

#ifdef SHADOWRECEIVER
        if (uLightCastsShadowsArray[i] > 0.0) {
            #if MAX_SHADOW_TEXTURES > 2
              if (uLightAttenuationArray[0].x > 0.0) {
                  colour *= (tmp > 0.002) ? GetShadow(i, uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations / 2) : 1.0;
              } else if (i == 0) {
                  colour *= (tmp > 0.002) ? CalcPSSMDepthShadow(pssmSplitPoints, lightSpacePosArray[0], lightSpacePosArray[1], lightSpacePosArray[2], shadowMap0, shadowMap1, shadowMap2, vDepth, uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations) : 1.0;
              } else {
                  colour *= (tmp > 0.002) ? GetShadow(i + 2, uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations) : 1.0;
              }
            #else
              colour *= (tmp > 0.002) ? GetShadow(i, uShadowDepthOffset, uShadowFilterSize, uShadowFilterIterations / 2) : 1.0;
            #endif
        }
#endif
        total_colour += colour;
    }

// Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    vec3 reflection = -normalize(reflect(v, n));
    total_colour += GetIBLContribution(diffuseColor, specularColor, roughness, NdotV, n, reflection);
#else
    total_colour += (uAmbientLightColour * albedo.rgb);
#endif

// Apply optional PBR terms for additional (optional) shading
#ifdef HAS_OCCLUSIONMAP
    total_colour *= (uSurfaceAmbientColour * texture2D(uOcclusionSampler, tex_coord, uLOD).r);
#endif

#ifdef HAS_EMISSIVEMAP
    total_colour += (uSurfaceEmissiveColour * SRGBtoLINEAR(texture2D(uEmissiveSampler, tex_coord, uLOD).rgb));
#endif

#ifdef HAS_REFLECTION
    total_colour = ApplyReflection(total_colour, n, v, metallic);
#endif //HAS_REFLECTION

    total_colour = ApplyFog(total_colour);

#ifdef HAS_ALPHA
    gl_FragColor = vec4(total_colour, albedo.a);
#else
    gl_FragColor = vec4(total_colour, 1.0);
#endif

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(uAlbedoSampler, vUV0.xy).a < 0.5) {
        discard;
    }
#endif //SHADOWCASTER_ALPHA
    gl_FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
#endif //SHADOWCASTER
}
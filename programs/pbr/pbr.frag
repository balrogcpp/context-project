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

#define MAX_LIGHTS 5
#define MAX_SHADOWS 3
#ifdef SHADOWCASTER_ALPHA
in vec2 vUV0;
uniform sampler2D uBaseColorSampler;
#endif
#ifndef SHADOWCASTER
in vec2 vUV0;
in float vDepth;
in float vAlpha;
#ifdef HAS_BASECOLORMAP
uniform sampler2D uBaseColorSampler;
#endif
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
#endif
#ifdef HAS_NORMALMAP
uniform sampler2D uNormalSampler;
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
#endif
#ifdef SHADOWRECEIVER
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform vec3 pssmSplitPoints;
in vec4 lightSpacePosArray[MAX_SHADOWS];
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
#ifdef REFLECTION
uniform sampler2D uReflectionMap;
uniform sampler2D uNoiseMap;
in vec4 projectionCoord;
#endif

const float M_PI = 3.141592653589793;

//SRGB corretion
#include "srgb.glsl"

//Shadows block
#ifdef SHADOWRECEIVER
#include "receiver.glsl"
//----------------------------------------------------------------------------------------------------------------------
float CalcPSSMDepthShadow()
{
    float camDepth = vDepth;

    // calculate shadow
    if (camDepth <= pssmSplitPoints.x)
    {
        return calcDepthShadow(shadowMap0, lightSpacePosArray[0]);
    }
    else if (camDepth <= pssmSplitPoints.y)
    {
        return calcDepthShadow(shadowMap1, lightSpacePosArray[1]);
    }
    else if (camDepth <= pssmSplitPoints.z)
    {
        return calcDepthShadow(shadowMap2, lightSpacePosArray[2]);
    }
    else
    {
        return 1.0;
    }
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
    vec3 n = texture2D(uNormalSampler, uv).xyz;
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
    metallic *= texture2D(uMetallicSampler, uv).r;
#endif

    return metallic;
}

//----------------------------------------------------------------------------------------------------------------------
float GetRoughness(vec2 uv) {
    const float c_MinRoughness = 0.04;
    float roughness = uSurfaceSpecularColour;

#ifdef HAS_ROUGHNESSMAP
    roughness *= texture2D(uRoughnessSampler, uv).r;
#endif

    return roughness > c_MinRoughness ? roughness : c_MinRoughness;
}

//----------------------------------------------------------------------------------------------------------------------
vec4 GetDiffuse(vec2 uv) {
    vec3 base_color = vec3(0.0);
    float alpha = 1.0;
#ifdef HAS_BASECOLORMAP
    vec4 tmp = texture2D(uBaseColorSampler, uv);
    base_color = SRGBtoLINEAR(tmp.rgb);
    alpha = tmp.a;
    base_color *= uSurfaceDiffuseColour;
#else
    base_color.rgb = SRGBtoLINEAR(uSurfaceDiffuseColour);
#endif
#ifdef HAS_COLOURS
    base_color.rgb *= vColor;
#endif
#ifdef PAGED_GEOMETRY
//    alpha *= vAlpha;
#endif

    if (alpha < uAlphaRejection) {
        discard;
    }

    return vec4(base_color, alpha);
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
#ifdef REFLECTION
vec3 ApplyReflection(vec3 color, vec3 n, vec3 v, float metallic) {
    vec4 projection = projectionCoord;
    const float fresnelBias = 0.1;
    const float fresnelScale = 1.8;
    const float fresnelPower = 8.0;
    const float noiseScale = 0.1;
    float cosa = dot(n, -v);
    float fresnel = fresnelBias + fresnelScale * pow(1.0 + cosa, fresnelPower);
    fresnel = clamp(fresnel, 0.0, 1.0);
    vec2 noise = texture2D(uNoiseMap, vUV0.xy).xy - 0.5;
    projection.xy += (noiseScale * noise);
    vec3 reflectionColour = texture2DProj(uReflectionMap, projection).rgb;
    return mix(color, reflectionColour, fresnel * metallic);
}
#endif

#ifdef HAS_PARALLAXMAP
//----------------------------------------------------------------------------------------------------------------------
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float scale = 0.01;
    const float offset = 0.0;
    float displacement =  texture2D(uOffsetSampler, texCoords).r * scale + offset;
    return texCoords - viewDir.xy * displacement;
}
#endif

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
//----------------------------------------------------------------------------------------------------------------------
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
//----------------------------------------------------------------------------------------------------------------------
vec3 Diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
//----------------------------------------------------------------------------------------------------------------------
vec3 SpecularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
//----------------------------------------------------------------------------------------------------------------------
float GeometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
//----------------------------------------------------------------------------------------------------------------------
float MicrofacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}
#endif //!SHADOWCASTER

//IBL PBR extention
#ifdef USE_IBL
// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
vec3 GetIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
    float mipCount = 9.0;// resolution of 512x512
    float lod = (pbrInputs.perceptualRoughness * mipCount);
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = SRGBtoLINEAR(texture2D(ubrdfLUT, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness))).rgb;
    vec3 diffuseLight = SRGBtoLINEAR(textureCube(uDiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
    vec3 specularLight = SRGBtoLINEAR(textureCubeLod(uSpecularEnvSampler, reflection, lod)).rgb;
#else
    vec3 specularLight = SRGBtoLINEAR(textureCube(uSpecularEnvSampler, reflection)).rgb;
#endif

    vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

    return diffuse + specular;
}
// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
vec3 GetIBLContribution(vec3 diffuseColor, vec3 specularColor, float perceptualRoughness, float NdotV, vec3 n, vec3 reflection)
{
    float mipCount = 9.0;// resolution of 512x512
    float lod = (perceptualRoughness * mipCount);
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = SRGBtoLINEAR(texture2D(ubrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness))).rgb;
    vec3 diffuseLight = SRGBtoLINEAR(textureCube(uDiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
    vec3 specularLight = SRGBtoLINEAR(textureCubeLod(uSpecularEnvSampler, reflection, lod)).rgb;
#else
    vec3 specularLight = SRGBtoLINEAR(textureCube(uSpecularEnvSampler, reflection)).rgb;
#endif

    vec3 diffuse = diffuseLight * diffuseColor;
    vec3 specular = specularLight * (specularColor * brdf.x + brdf.y);

    return diffuse + specular;
}
#endif

void main()
{
#ifndef SHADOWCASTER
    vec3 v = normalize(uCameraPosition - vPosition);
    vec2 tex_coord = vUV0.xy;

#ifdef TERRAIN
    tex_coord *= 100.0;
#endif

#ifdef HAS_PARALLAXMAP
    tex_coord = ParallaxMapping(tex_coord, v);
#endif

    // The albedo may be defined from a base texture or a flat color
    vec4 baseColor = GetDiffuse(tex_coord);
    float metallic = GetMetallic(tex_coord);
    float roughness = GetRoughness(tex_coord);

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = roughness * roughness;

    const vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= (1.0 - metallic);
    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

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

        vec3 vLightView = uLightPositionArray[i] - vPosition;
        float fLightD = length(vLightView);
        vLightView = normalize(vLightView);

        vec4 vAttParams = uLightAttenuationArray[i];

        float fAtten = 1.0;
        float fSpotT = 1.0;

        if (fLightD > vAttParams.x) {
            continue;
        }

        if (vAttParams.z != 0.0) {
            vec3 vSpotParams = uLightSpotParamsArray[i];

            fAtten  = 1.0 / (vAttParams.y + vAttParams.z*fLightD + vAttParams.w*fLightD*fLightD);

            float rho = dot(l, vLightView);
            float fSpotE = clamp((rho - vSpotParams.y) / (vSpotParams.x - vSpotParams.y), 0.0, 1.0);
            fSpotT = pow(fSpotE, vSpotParams.z);
        }

        float ndotl = dot(n, l);
        float NdotL = clamp(ndotl, 0.001, 1.0);
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
        alphaRoughness,
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

#ifdef SHADOWRECEIVER
        if (uLightCastsShadowsArray[i] == 1.0) {
            tmp *= CalcPSSMDepthShadow();
        }
#endif

        total_colour += (tmp * uLightDiffuseScaledColourArray[i] * (diffuseContrib + specContrib));
    }

// Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    vec3 reflection = -normalize(reflect(v, n));
    total_colour += GetIBLContribution(diffuseColor, specularColor, roughness, NdotV, n, reflection);
#else
    total_colour += ((uSurfaceAmbientColour + uAmbientLightColour) * baseColor.rgb);
#endif

// Apply optional PBR terms for additional (optional) shading
#ifdef HAS_OCCLUSIONMAP
    total_colour *= texture2D(uOcclusionSampler, tex_coord).r;
#endif

#ifdef HAS_EMISSIVEMAP
    total_colour += SRGBtoLINEAR(texture2D(uEmissiveSampler, tex_coord).rgb + uSurfaceEmissiveColour);
#else
    total_colour += SRGBtoLINEAR(uSurfaceEmissiveColour);
#endif

#ifdef REFLECTION
    total_colour = ApplyReflection(total_colour, n, v, metallic);
#endif //REFLECTION

    total_colour = ApplyFog(total_colour);
    gl_FragColor = vec4(total_colour, baseColor.a);

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
    if (texture2D(uBaseColorSampler, vUV0.xy).a < 0.5) {
        discard;
    }
#endif //SHADOWCASTER_ALPHA
    gl_FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
#endif //SHADOWCASTER
}
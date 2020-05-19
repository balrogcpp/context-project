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
#define USE_TEX_LOD
#if VERSION != 120
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#version 100
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision highp float;
#define in varying
#define out varying
#endif

#ifdef SHADOWCASTER_ALPHA
in vec4 vUV;
#ifdef HAS_BASECOLORMAP
uniform sampler2D uBaseColorSampler;
uniform float uAlphaRejection;
#endif
#endif

#ifndef SHADOWCASTER
in vec4 vUV;
#ifdef HAS_BASECOLORMAP
uniform sampler2D uBaseColorSampler;
#endif

#define MAX_LIGHTS 5
uniform float uAlphaRejection;
uniform vec4 uSurfaceAmbientColour;
uniform vec4 uSurfaceDiffuseColour;
uniform vec4 uSurfaceSpecularColour;
uniform vec4 uSurfaceEmissiveColour;
uniform float uSurfaceShininessColour;
uniform vec3 uAmbientLightColour;
uniform float uLightCount;
uniform vec3 uLightPositionArray[MAX_LIGHTS];
uniform vec3 uLightPositionObjectSpaceArray[MAX_LIGHTS];
uniform vec3 uLightPositionViewSpaceArray[MAX_LIGHTS];
uniform vec3 uLightDirectionArray[MAX_LIGHTS];
uniform vec3 uLightDiffuseScaledColourArray[MAX_LIGHTS];
uniform vec3 uLightSpecularScalesColourArray[MAX_LIGHTS];
uniform vec4 uLightAttenuationArray[MAX_LIGHTS];
uniform vec3 uLightSpotParamsArray[MAX_LIGHTS];
#ifdef SHADOWRECEIVER
uniform float uLightCastsShadowsArray[MAX_LIGHTS];
uniform float bias0;
uniform float bias1;
uniform float bias2;
#endif
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform vec3 uCameraPosition;

#ifdef USE_IBL
uniform samplerCube uDiffuseEnvSampler;
uniform samplerCube uSpecularEnvSampler;
uniform sampler2D ubrdfLUT;
#endif

#ifdef HAS_NORMALMAP
uniform sampler2D uNormalSampler;
#endif
#ifdef HAS_EMISSIVEMAP
uniform sampler2D uEmissiveSampler;
#endif
#ifdef HAS_METALROUGHNESSMAP
#ifdef HAS_SEPARATE_ROUGHNESSMAP
uniform sampler2D uMetallicSampler;
uniform sampler2D uRoughnessSampler;
#else
uniform sampler2D uMetallicRoughnessSampler;
#endif
#else
#endif
#ifdef HAS_OCCLUSIONMAP
uniform sampler2D uOcclusionSampler;
#endif

#ifdef SHADOWRECEIVER
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform vec4 inverseShadowmapSize0;
uniform vec4 inverseShadowmapSize1;
uniform vec4 inverseShadowmapSize2;
uniform vec4 pssmSplitPoints;
uniform vec4 uShadowColour;
in vec4 lightSpacePosArray[MAX_LIGHTS * 3];
#endif

in vec3 vPosition;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
in mat3 vTBN;
#else
in vec3 vNormal;
#endif
#endif
#ifdef REFLECTION
uniform sampler2D uReflectionMap;
in vec4 projectionCoord;
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif

#ifdef SHADOWRECEIVER
vec4 offsetSample(vec4 uv, vec2 offset, float invMapSize)
{
    return vec4(uv.xy + offset * invMapSize * uv.w, uv.z, uv.w);
}

float calcDepthShadow_Poisson(sampler2D shadowMap, vec4 uv, vec4 invShadowMapSize, float bias)
{
    // 4-sample PCF
    uv.xyz /= uv.w;

    uv.z = uv.z * 0.5 + 0.5; // convert -1..1 to 0..1
    float shadow = 0.0;
    float compare = uv.z;
    int counter = 0;

//    float shadow_depth = texture2DProj(shadowMap, uv).r - bias;
//    shadow = (shadow_depth > compare) ? 1.0 : 0.0;

//    const float radius = 1.0;
//    for (float y = -radius; y < radius; y++)
//    for (float x = -radius; x < radius; x++)
//    {
//        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy;
//        float depth = texture2D(shadowMap, uv2).r - bias;
//        if (depth > compare) {
//            counter++;
//        }
//    }
//    shadow = counter / ( (2.0 * radius) * (2.0 * radius));

//    const float radius = 1.0;
//    for (float y = -radius; y <= radius; y++)
//    for (float x = -radius; x <= radius; x++)
//    {
//        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy;
//        float depth = texture2D(shadowMap, uv2).r - bias;
//        if (depth > compare) {
//            counter++;
//        }
//    }
//    shadow = counter / ( (2.0 * radius + 1.0 ) * (2.0 * radius + 1.0));

//    const int iterations = 16;
//    const vec2 poissonDisk16[16] = vec2[](
//    vec2( -0.94201624, -0.39906216 ),
//    vec2( 0.94558609, -0.76890725 ),
//    vec2( -0.094184101, -0.92938870 ),
//    vec2( 0.34495938, 0.29387760 ),
//    vec2( -0.91588581, 0.45771432 ),
//    vec2( -0.81544232, -0.87912464 ),
//    vec2( -0.38277543, 0.27676845 ),
//    vec2( 0.97484398, 0.75648379 ),
//    vec2( 0.44323325, -0.97511554 ),
//    vec2( 0.53742981, -0.47373420 ),
//    vec2( -0.26496911, -0.41893023 ),
//    vec2( 0.79197514, 0.19090188 ),
//    vec2( -0.24188840, 0.99706507 ),
//    vec2( -0.81409955, 0.91437590 ),
//    vec2( 0.19984126, 0.78641367 ),
//    vec2( 0.14383161, -0.14100790 )
//    );
//
//    for (int i = 0; i < iterations; i++)
//    {
//        vec2 uv2 = uv.xy + poissonDisk16[i] * invShadowMapSize.x;
//        float shadow_depth = texture2D(shadowMap, uv2).r - bias;
//        if (shadow_depth > compare) {
//          counter++;
//        }
//    }
//    shadow = counter / iterations;

//    const float radius = 1.0;
//    const int iterations = 16;
//    const vec2 poissonDisk16[16] = vec2[](
//    vec2( -0.94201624, -0.39906216 ),
//    vec2( 0.94558609, -0.76890725 ),
//    vec2( -0.094184101, -0.92938870 ),
//    vec2( 0.34495938, 0.29387760 ),
//    vec2( -0.91588581, 0.45771432 ),
//    vec2( -0.81544232, -0.87912464 ),
//    vec2( -0.38277543, 0.27676845 ),
//    vec2( 0.97484398, 0.75648379 ),
//    vec2( 0.44323325, -0.97511554 ),
//    vec2( 0.53742981, -0.47373420 ),
//    vec2( -0.26496911, -0.41893023 ),
//    vec2( 0.79197514, 0.19090188 ),
//    vec2( -0.24188840, 0.99706507 ),
//    vec2( -0.81409955, 0.91437590 ),
//    vec2( 0.19984126, 0.78641367 ),
//    vec2( 0.14383161, -0.14100790 )
//    );
//
//    for (float y = -radius; y <= radius; y++)
//    for (float x = -radius; x <= radius; x++)
//    for (int i = 0; i < iterations; i++)
//    {
//        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy + poissonDisk16[i] * invShadowMapSize.x;
//        float depth = texture2D(shadowMap, uv2).r - bias;
//        if (depth > compare) {
//          counter++;
//        }
//    }
//    shadow = counter / ( iterations * (2 * radius + 1 ) * (2 * radius + 1));

    const float radius = 1.0;
    const int iterations = 4;
    const vec2 poissonDisk16[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
    );

    for (float y = -radius; y < radius; y++)
    for (float x = -radius; x < radius; x++)
    for (int i = 0; i < iterations; i++)
    {
        vec2 uv2 = uv.xy + vec2(x, y) * invShadowMapSize.xy + poissonDisk16[i] * invShadowMapSize.x;
        float depth = texture2D(shadowMap, uv2).r - bias;
        if (depth > compare) {
          counter++;
        }
    }
    shadow = counter / ( iterations * (2 * radius ) * (2 * radius));

    shadow = clamp(shadow + uShadowColour.r, 0, 1);

    return shadow;
}

float calcPSSMDepthShadow()
{
    float camDepth = vUV.z;

    // calculate shadow
    if (camDepth <= pssmSplitPoints.x)
    {
        return calcDepthShadow_Poisson(shadowMap0, lightSpacePosArray[0], inverseShadowmapSize0, bias0);
    }
    else if (camDepth <= pssmSplitPoints.y)
    {
        return calcDepthShadow_Poisson(shadowMap1, lightSpacePosArray[1], inverseShadowmapSize1, bias1);
    }
    else
    {
        return calcDepthShadow_Poisson(shadowMap2, lightSpacePosArray[2], inverseShadowmapSize2, bias2);
    }
}
#endif

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
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

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

#define MANUAL_SRGB
#define SRGB_FAST_APPROXIMATION
//#define SRGB_SQRT

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_FAST_APPROXIMATION
#ifdef SRGB_SQRT
    vec3 linOut = srgbIn.rgb * srgbIn.rgb;
#else
    vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#endif
#else //SRGB_FAST_APPROXIMATION
    vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
    vec3 linOut = mix(srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
#endif //SRGB_FAST_APPROXIMATION
    return vec4(linOut,srgbIn.w);
#else //MANUAL_SRGB
    return srgbIn;
#endif //MANUAL_SRGB
}


vec3 SRGBtoLINEAR(vec3 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_FAST_APPROXIMATION
#ifdef SRGB_SQRT
    vec3 linOut = srgbIn.xyz * srgbIn.xyz;
#else
    vec3 linOut = pow(srgbIn.rgb, vec3(2.2));
#endif
#else //SRGB_FAST_APPROXIMATION
    vec3 bLess = step(vec3(0.04045),srgbIn.rgb);
    vec3 linOut = mix( srgbIn.rgb/vec3(12.92), pow((srgbIn.rgb+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
#endif //SRGB_FAST_APPROXIMATION
    return linOut;
#else //MANUAL_SRGB
    return srgbIn;
#endif //MANUAL_SRGB
}

//------------------------------------------------------------------------------
vec4 LINEARtoSRGB(vec4 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_SQRT
    return vec4(sqrt(srgbIn.rgb), srgbIn.a);
#else
    return vec4(pow(srgbIn.rgb, vec3(1.0 / 2.2)), srgbIn.a);
#endif
#else
    return srgbIn;
#endif
}

//-----------------------------------------------------------------------------
vec3 LINEARtoSRGB(vec3 srgbIn)
{
#ifdef MANUAL_SRGB
#ifdef SRGB_SQRT
    return sqrt(srgbIn);
#else
    return pow(srgbIn.rgb, vec3(1.0 / 2.2));
#endif
#else
    return srgbIn;
#endif
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
    // Retrieve the tangent space matrix
#ifndef HAS_TANGENTS
    vec3 pos_dx = dFdx(vPosition);
    vec3 pos_dy = dFdy(vPosition);
    vec3 tex_dx = dFdx(vec3(vUV.xy, 0.0));
    vec3 tex_dy = dFdy(vec3(vUV.xy, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

#ifdef HAS_NORMALS
    vec3 ng = normalize(vNormal);
#else
    vec3 ng = cross(pos_dx, pos_dy);
#endif

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
#else // HAS_TANGENTS
    mat3 tbn = vTBN;
#endif

#ifdef HAS_NORMALMAP
    vec3 n = texture2D(uNormalSampler, vUV.xy).rgb;
    n = normalize(tbn * ((2.0 * n - 1.0)));
#else
    vec3 n = tbn[2].xyz;
#endif

    return n;
}

#ifdef USE_IBL
// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
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
vec3 getIBLContribution(vec3 diffuseColor, vec3 specularColor, float perceptualRoughness, float NdotV, vec3 n, vec3 reflection)
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

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
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
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

#ifdef HAS_PARALLAX
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture2D(uBaseColorSampler, texCoords).a;
    const float height_scale = 0.01;
    vec2 p = viewDir.xy * (height * height_scale);
    return texCoords - p;
}
#endif

#endif //SHADOWCASTER

void main()
{
#ifndef SHADOWCASTER
    vec3 v = normalize(uCameraPosition - vPosition);
//    vec3 v = normalize(vPosition - uCameraPosition);

#ifdef HAS_PARALLAX
    vec2 tex_coord = ParallaxMapping(vUV.xy, v);
#else
    vec2 tex_coord = vUV.xy;
#endif

    // The albedo may be defined from a base texture or a flat color
#ifdef HAS_BASECOLORMAP
    vec4 baseColor = SRGBtoLINEAR(texture2D(uBaseColorSampler, tex_coord)) * uSurfaceDiffuseColour;
#else
    vec4 baseColor = SRGBtoLINEAR(uSurfaceDiffuseColour);
#endif

    float alpha = baseColor.a;

#ifdef FADE
    alpha *= vUV.w;
#endif

    if (alpha < uAlphaRejection) {
        discard;
    }

    float metallic = 1.0;
    float perceptualRoughness = 1.0;

#ifdef HAS_METALROUGHNESSMAP
#ifdef HAS_SEPARATE_ROUGHNESSMAP
    vec4 mrSampleMetallic = texture2D(uMetallicSampler, tex_coord);
    vec4 mrSampleRoughness = texture2D(uRoughnessSampler, tex_coord);
    perceptualRoughness = mrSampleRoughness.r;
    metallic = mrSampleMetallic.r;
#else
    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    vec4 mrSample = texture2D(uMetallicRoughnessSampler, tex_coord);
    metallic = mrSample.r;
    perceptualRoughness = mrSample.a;
#endif
#endif

    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    const vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0) * reflectance90;

    vec3 n = getNormal();                             // normal at surface point
    float NdotV = abs(dot(n, v)) + 0.001;
    vec3 reflection = -normalize(reflect(v, n));

    vec3 total_colour = vec3(0.0);

#ifdef SHADOWRECEIVER
    float attenuation = 1.0;
#else
    float attenuation = 1.0;
#endif

    for (int i = 0; i < int(uLightCount); i++)
    {
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

        if (vAttParams.z != 0) {
            vec3 vSpotParams = uLightSpotParamsArray[i];

            fAtten  = 1.0 / (vAttParams.y + vAttParams.z*fLightD + vAttParams.w*fLightD*fLightD);

            float rho        = dot(l, vLightView);
            float fSpotE    = clamp((rho - vSpotParams.y) / (vSpotParams.x - vSpotParams.y), 0.0, 1.0);
            fSpotT    = pow(fSpotE, vSpotParams.z);
        }

        float NdotL = clamp(dot(n, l), 0.001, 1.0);
        float NdotH = clamp(dot(n, h), 0.0, 1.0);
        float LdotH = clamp(dot(l, h), 0.0, 1.0);
        float VdotH = clamp(dot(v, h), 0.0, 1.0);

        PBRInfo pbrInputs = PBRInfo(
        NdotL,
        NdotV,
        NdotH,
        LdotH,
        VdotH,
        perceptualRoughness,
        metallic,
        specularEnvironmentR0,
        specularEnvironmentR90,
        alphaRoughness,
        diffuseColor,
        specularColor
        );

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = specularReflection(pbrInputs);
        float G = geometricOcclusion(pbrInputs);
        float D = microfacetDistribution(pbrInputs);

        // Calculation of analytical lighting contribution
        vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
        vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);

        vec3 color = NdotL * uLightDiffuseScaledColourArray[i] * (diffuseContrib + specContrib) * fSpotT * fAtten;

#ifdef SHADOWRECEIVER
        float shadow = 1.0;
        if (uLightCastsShadowsArray[i] == 1.0) {
            shadow = calcPSSMDepthShadow();
            attenuation = min(attenuation, shadow);
        } else {
            attenuation = 1.0;
        }
#endif

        total_colour += color;
    }

        // Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    total_colour += getIBLContribution(diffuseColor, specularColor, perceptualRoughness, NdotV, n, reflection) / attenuation;
#else
    total_colour += (uAmbientLightColour * baseColor.rgb) / attenuation;
#endif

        // Apply optional PBR terms for additional (optional) shading
#ifdef HAS_OCCLUSIONMAP
    total_colour *= texture2D(uOcclusionSampler, tex_coord).r;
#endif

#ifdef HAS_EMISSIVEMAP
    total_colour += SRGBtoLINEAR(texture2D(uEmissiveSampler, tex_coord).rgb);
#else
    total_colour += SRGBtoLINEAR(uSurfaceEmissiveColour.rgb);
#endif

#ifdef REFLECTION
    vec2 final = projectionCoord.xy / projectionCoord.w;

    vec3 reflectionColour = texture2D(uReflectionMap , final).rgb;
    const float fresnelBias = 0.1;
    const float fresnelScale = 1.8;
    const float fresnelPower = 8.0;

    float vector = dot(n, -v);
    float fresnel = fresnelBias + fresnelScale * pow(1.0 + vector, fresnelPower);

    total_colour = mix(total_colour, reflectionColour, fresnel * metallic);
#endif

    gl_FragColor = vec4(total_colour, attenuation);

#else //SHADOWCASTER
#ifdef SHADOWCASTER_ALPHA
    float alpha = texture2D(uBaseColorSampler, vUV.xy).a;

    if (alpha < 0.5) {
        discard;
    }
#endif

    gl_FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
#endif //SHADOWCASTER
}
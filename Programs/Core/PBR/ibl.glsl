// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef IBL_GLSL
#define IBL_GLSL

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
//----------------------------------------------------------------------------------------------------------------------
vec3 GetIBLContribution(const sampler2D ubrdfLUT, const samplerCube uDiffuseEnvSampler, const samplerCube uSpecularEnvSampler, const vec3 diffuseColor, const vec3 specularColor, const float perceptualRoughness, const float NdotV, const vec3 n, const vec3 reflection)
{
  // retrieve a scale and bias to F0. See [1], Figure 3
  vec3 brdf = SRGBtoLINEAR(texture2D(ubrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness))).rgb;
  vec3 diffuseLight = SRGBtoLINEAR(textureCube(uDiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
  vec3 specularLight = SRGBtoLINEAR(textureCubeLod(uSpecularEnvSampler, reflection, perceptualRoughness * 9.0)).rgb;
#else
  vec3 specularLight = SRGBtoLINEAR(textureCube(uSpecularEnvSampler, reflection)).rgb;
#endif

  vec3 diffuse = (diffuseLight * diffuseColor);
  vec3 specular = specularLight * ((specularColor * brdf.x) + brdf.y);

  return diffuse + specular;
}

#endif
